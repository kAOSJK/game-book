#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "lib.h"

#define CHAR_SEPARATOR "$"

void begin_chapter(int y_max, int x_max, int speed_0, int speed_1, chapter *chap, unsigned int chapter_index, array_list *parsed_story, char *name, char *buffer, char *usr_buffer)
{
    FILE *fp;
    WINDOW *win;
    part *current_part;
    char **text = NULL;
    char **temp = NULL;
    char *key = NULL;
    /* WINDOWs */
    WINDOW *agilitywin;
    WINDOW *mentalwin;
    WINDOW *trustwin;

    /* TEST VARs */
    int test_difficulty;
    bool result;

    /* UPGRADE VARs */
    char *skill_type = NULL;
    int add_value;

    /* CREATE A WINDOW FOR OUR INPUT */
    win = create_newwin(8, x_max - 4, y_max - 9, 2);

    /* RELOAD WINDOWS */
    /* TODO: implement this */
    reload_windows_vars(y_max, x_max, get_json_data_int("agility", usr_buffer), get_json_data_int("mental", usr_buffer), get_json_data_int("trust", usr_buffer), agilitywin, mentalwin, trustwin);

    /* GET FIRST CHAPTER KEY */
    key = get_first_key(chapter_index, buffer);

    if (key != NULL)
        current_part = get_part_data(parsed_story, key, chapter_index, buffer);

    if (current_part != NULL)
        text = sentence_separator(current_part->text, CHAR_SEPARATOR);

    while (text != NULL)
    {
        /* WRITE STORY */
        write_text(text, win, y_max, x_max, speed_0, speed_1, name);
        clear_win(win);

        /* FREE KEY */
        free(key);
        key = NULL;

        if (current_part->upgrade != NULL)
        {
            add_value = json_object_get_int(json_object_object_get(current_part->upgrade, "add_value"));

            skill_type = strdup(json_object_get_string(json_object_object_get(current_part->upgrade, "skill_type")));

            if (strcmp(skill_type, "agility") == 0)
            {
                add_agility_value(add_value, fp, usr_buffer);
                /*
                printw("\nI normally added %d to %s in the user.json file\n", add_value, skill_type);*/
            }
            else if (strcmp(skill_type, "mental") == 0)
            {
                add_mental_value(add_value, fp, usr_buffer);
                /*
                printw("\nI normally added %d to %s in the user.json file\n", add_value, skill_type);*/
            }
            else if (strcmp(skill_type, "trust") == 0)
            {
                add_trust_value(add_value, fp, usr_buffer);
                /*
                printw("\nI normally added %d to %s in the user.json file\n", add_value, skill_type);*/
            }

            refresh();

            free(skill_type);
            skill_type = NULL;
        }

        if (current_part->next_key != NULL && *current_part->next_key != 0)
        {
            /* mean that the next key is linear */
            key = strdup(current_part->next_key);
        }
        else if (current_part->choices != NULL)
        {
            /* mean that the next key depends on player's choice */
            key = get_user_choices(win, current_part->choices, usr_buffer);
        }
        else if (current_part->test != NULL)
        {
            /* mean that the next key depends on player's result on a test */
            test_difficulty = json_object_get_int(json_object_object_get(current_part->test, "difficulty"));

            /* stop displaying win (during the test) */
            werase(win);
            wrefresh(win);

            result = agility(y_max, x_max, test_difficulty);

            if (result == true) /* mean that the player succeed the test */
                key = strdup((char *)json_object_get_string(json_object_object_get(current_part->test, "win_next_key")));
            else /* mean that the player failed the test */
                key = strdup((char *)json_object_get_string(json_object_object_get(current_part->test, "lose_next_key")));
            clear_win(win);
        }

        /* FREE CURRENT PART */
        free_part(current_part);
        current_part = NULL;

        /* FREE TEXT */
        for (temp = text; *temp; ++temp)
            free(*temp);
        free(text);
        text = NULL;

        /* mean that this is the end of this chapter */
        if (key == NULL)
            break;

        current_part = get_part_data(parsed_story, key, chapter_index, buffer);
        if (current_part == NULL)
        {
            /* FREE KEY */
            free(key);
            key = NULL;

            fprintf(stderr, "error: can't find part with the key %s\n", key);
            break;
        }

        text = sentence_separator(current_part->text, CHAR_SEPARATOR);
    }

    destroy_win(win);
    /*
    destroy_win(agilitywin);
    destroy_win(mentalwin);
    destroy_win(trustwin);*/
}

void write_text(char **story, WINDOW *win, int y_max, int x_max, int speed_0, int speed_1, char *name)
{
    size_t i, j;
    bool skip = false;
    int wait_time = speed_0;
    /* NAME VARs */
    unsigned int k = 0;
    unsigned int wrote_name;
    unsigned int n = 0;

    keypad(win, true);

    while (name[n + 1])
        n++;

    while (1)
    {
        for (j = 0; story[j]; j++)
        {
            wrote_name = 0;
            for (i = 0; story[j][i]; i++)
            {
                if (!skip)
                {
                    nodelay(win, true);
                    if (wgetch(win) == 10)
                        wait_time = speed_1;
                }

                fflush(stdout);
                usleep(wait_time);
                if (story[j][i] == '&')
                {
                    while (name[k])
                    {
                        mvwprintw(win, 1 + j, 2 + i + k + (n * wrote_name), "%c", name[k]);
                        fflush(stdout);
                        usleep(wait_time);
                        k++;
                    }
                    k = 0;
                    wrote_name++;
                }

                if (!wrote_name)
                    mvwprintw(win, 1 + j, 2 + i, "%c", story[j][i]);
                else
                {
                    if (story[j][i] != '&')
                        mvwprintw(win, 1 + j, 2 + i + (n * wrote_name), "%c", story[j][i]);
                }

                wrefresh(win);
            }
        }
        nodelay(win, false);

        if (wgetch(win) == 10)
            break;
    }
}

char *get_user_choices(WINDOW *win, array_list *choices, char *usr_buffer)
{
    /* DATA VARs */
    char *answer = NULL;                  /* the final choice text (next key) */
    char *option = NULL;                  /* text of a choice that the player can made */
    unsigned int options_length = 0;      /* option length not caring about player skills */
    unsigned int real_options_length = 0; /* real player option length */
    int choice = 0;                       /* player selection */
    int highlight = 0;                    /* to hightlight the right player selection */
    int i = 0;

    /* SPECIAL CHOICES VARs */
    json_object *obj_needed_skill = NULL;
    json_object *obj_needed_value = NULL;
    char *type = NULL;
    bool encounter_sc = false;

    /* ACTIVATE ARROW KEYS */
    keypad(win, true);

    /* ACTIVATE NCURSES COLORS */
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);

    options_length = (int)array_list_length(choices);
    for (i = 0; i < options_length; i++)
    {
        json_object_object_get_ex(array_list_get_idx(choices, i), "needed_skill", &obj_needed_skill);
        if (obj_needed_skill)
        {
            encounter_sc = true;
            json_object_object_get_ex(array_list_get_idx(choices, i), "needed_value", &obj_needed_value);
            type = (char *)json_object_get_string(obj_needed_skill);
            if (json_object_get_int(obj_needed_value) > 0)
            {
                if (get_json_data_int(type, usr_buffer) >= json_object_get_int(obj_needed_value))
                    real_options_length++;
            }
            else if (json_object_get_int(obj_needed_value) < 0)
            {
                if (get_json_data_int(type, usr_buffer) <= ((int)json_object_get_int(obj_needed_value) * -1))
                    real_options_length++;
            }
        }
        else
            real_options_length++;
    }

    if (encounter_sc == false)
        real_options_length = options_length;

    while (1)
    {
        for (i = 0; i < real_options_length; i++)
        {
            option = get_json_object_string("text", json_object_to_json_string(array_list_get_idx(choices, i)));

            if (i == highlight)
                wattron(win, A_REVERSE);

            json_object_object_get_ex(array_list_get_idx(choices, i), "needed_skill", &obj_needed_skill);
            if (obj_needed_skill == NULL)
                mvwprintw(win, i + 1, 2, "%s", option);
            else
            {
                json_object_object_get_ex(array_list_get_idx(choices, i), "needed_value", &obj_needed_value);
                type = (char *)json_object_get_string(obj_needed_skill);

                if (json_object_get_int(obj_needed_value) > 0)
                {
                    if (get_json_data_int(type, usr_buffer) >= json_object_get_int(obj_needed_value))
                    {
                        wattron(win, COLOR_PAIR(1));
                        mvwprintw(win, i + 1, 2, "%s", option);
                        wattroff(win, COLOR_PAIR(1));
                    }
                }
                else if (json_object_get_int(obj_needed_value) < 0)
                {
                    if (get_json_data_int(type, usr_buffer) <= ((int)json_object_get_int(obj_needed_value) * -1))
                    {
                        wattron(win, COLOR_PAIR(1));
                        mvwprintw(win, i + 1, 2, "%s", option);
                        wattroff(win, COLOR_PAIR(1));
                    }
                }
            }
            wattroff(win, A_REVERSE);

            free(option);
            option = NULL;
        }

        choice = wgetch(win);

        switch (choice)
        {
        case KEY_DOWN:
            highlight == real_options_length - 1 ? highlight = 0 : highlight++;
            break;
        case KEY_UP:
            highlight == 0 ? highlight = real_options_length - 1 : highlight--;
            break;
        default:
            break;
        }
        if (choice == 10)
            break;
        refresh();
    }

    free(option);
    option = NULL;

    answer = get_json_object_string("next_key", json_object_to_json_string(array_list_get_idx(choices, highlight)));
    if (answer == NULL)
        return NULL;

    /* ACTIVATE ARROW KEYS */
    keypad(win, false);

    clear_win(win);

    return answer;
}
