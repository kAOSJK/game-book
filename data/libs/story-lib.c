#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "lib.h"

#define CHAR_SEPARATOR "$"

void begin_chapter(int y_max, int x_max, int speed_0, int speed_1, int agility_speed, chapter *chap, unsigned int chapter_index, array_list *parsed_story, char *name, char *buffer, char *usr_buffer)
{
    /* WINDOWS VARs */
    FILE *fp;
    WINDOW *topwin;
    WINDOW *win;
    WINDOW *bottomwin;
    int top_height = y_max / 4;
    int height = y_max / 2 - (y_max / 10);
    int width = x_max - 4;
    int bottom_height = y_max / 4;
    /* BASICS VARs*/
    char *title = NULL;
    part *current_part;
    char **text = NULL;
    char **temp = NULL;
    char *key = NULL;
    /* TEST VARs */
    int test_difficulty;
    bool result;
    /* UPGRADE VARs */
    char *skill_type = NULL;
    int add_value;
    /* OTHERS VARs */
    char *new_date = NULL;
    char *date = NULL;
    unsigned int i;

    /* CREATE A WINDOW FOR OUR INPUT */
    topwin = create_newwin(top_height, width, 1, 2);
    win = create_newwin(height, width, y_max - (height + ((height / 4) * 3)), 2);
    bottomwin = create_newwin(bottom_height, width, y_max - bottom_height - 1, 2);

    /* GET FIRST CHAPTER KEY */
    key = get_first_key(chapter_index, buffer);

    if (key != NULL)
        current_part = get_part_data(parsed_story, key, chapter_index, buffer);

    if (current_part != NULL)
        text = sentence_separator(current_part->text, CHAR_SEPARATOR);

    for (i = 1; i <= 5; i++)
        mvwprintw(topwin, i, width / 3, "%c", '|');

    /* TODO: CHANGE ORDER OF WRITING DATE DEPENDING ON THE CHOOSEN LANGUAGE */
    /* GET AND WRITE DATE */
    date = get_part_date(current_part);
    if (date != NULL)
    {
        for (i = 0; date[i]; i++)
        {
            mvwprintw(topwin, top_height / 2, (((width / 3) / 2) - (strlen(date) / 2)) + i, "%c", (IS_LOWER_CASE(date[i]) ? date[i] - 32 : date[i]));
            fflush(stdout);
            usleep(speed_0); /* speed_0 * 10 */
            wrefresh(topwin);
        }
    }

    /* GET AND WRITE TITLE */
    title = get_array_idx_key((char *)json_object_to_json_string(array_list_get_idx(parsed_story, chapter_index)));
    if (title != NULL)
    {
        for (i = 0; title[i]; i++)
        {
            mvwprintw(topwin, top_height / 2, (width / 2) + ((width / 3) / 2) - (strlen(title) / 2) + i, "%c", (IS_LOWER_CASE(title[i]) ? title[i] - 32 : title[i]));
            fflush(stdout);
            usleep(speed_0); /* speed_0 * 10 */
            wrefresh(topwin);
        }
    }

    /* WRITE THE CHAPTER */
    while (text != NULL)
    {
        for (i = 1; i <= 5; i++)
        {
            mvwprintw(topwin, i, width / 3, "%c", '|');
        }

        /* WRITE TITLE */
        if (title != NULL)
        {
            for (i = 0; title[i]; i++)
                mvwprintw(topwin, top_height / 2, (width / 2) + ((width / 3) / 2) - (strlen(title) / 2) + i, "%c", (IS_LOWER_CASE(title[i]) ? title[i] - 32 : title[i]));
            wrefresh(topwin);
        }

        /* WRITE DATE */
        if (current_part->date != NULL)
        {
            new_date = get_part_date(current_part);

            /* WRITE IT IF ITS NEW */
            if (strcmp(date, new_date) != 0)
            {
                free(date);
                date = NULL;

                date = get_part_date(current_part);
                for (i = 0; date[i]; i++)
                {
                    mvwprintw(topwin, top_height / 2, (((width / 3) / 2) - (strlen(date) / 2)) + i, "%c", (IS_LOWER_CASE(date[i]) ? date[i] - 32 : date[i]));
                    fflush(stdout);
                    usleep(speed_0); /* speed_0 * 10 */
                    wrefresh(topwin);
                }
            }

            free(new_date);
            new_date = NULL;
        }
        else if (date != NULL)
        {
            for (i = 0; date[i]; i++)
                mvwprintw(topwin, top_height / 2, (((width / 3) / 2) - (strlen(date) / 2)) + i, "%c", (IS_LOWER_CASE(date[i]) ? date[i] - 32 : date[i]));
            wrefresh(topwin);
        }

        /* WRITE STORY */
        write_text(text, win, y_max, x_max, speed_0, speed_1, name);

        /* FREE KEY */
        free(key);
        key = NULL;

        if (current_part->upgrade != NULL)
        {
            add_value = json_object_get_int(json_object_object_get(current_part->upgrade, "add_value"));

            skill_type = strdup(json_object_get_string(json_object_object_get(current_part->upgrade, "skill_type")));

            if (strcmp(skill_type, "agility") == 0)
                add_agility_value(add_value, usr_buffer);
            else if (strcmp(skill_type, "mental") == 0)
                add_mental_value(add_value, usr_buffer);
            else if (strcmp(skill_type, "trust") == 0)
                add_trust_value(add_value, usr_buffer);

            free(skill_type);
            skill_type = NULL;
        }

        if (current_part->next_key != NULL && *current_part->next_key != 0)
        {
            clear_win(win);

            /* mean that the next key is linear */
            key = strdup(current_part->next_key);
        }
        else if (current_part->choices != NULL)
        {
            /* mean that the next key depends on player's choice */
            key = get_user_choices(bottomwin, current_part->choices, usr_buffer);

            /* reload win */
            box(win, 0, 0);
            wrefresh(win);
        }
        else if (current_part->test != NULL)
        {
            clear_win(win);

            /* mean that the next key depends on player's result on a test */
            test_difficulty = json_object_get_int(json_object_object_get(current_part->test, "difficulty"));

            /* stop displaying all windows (during the test) */
            werase(topwin);
            wrefresh(topwin);

            werase(win);
            wrefresh(win);

            werase(bottomwin);
            wrefresh(bottomwin);

            result = agility(y_max, x_max, test_difficulty, agility_speed);

            if (result == true) /* mean that the player succeed the test */
                key = strdup((char *)json_object_get_string(json_object_object_get(current_part->test, "win_next_key")));
            else /* mean that the player failed the test */
                key = strdup((char *)json_object_get_string(json_object_object_get(current_part->test, "lose_next_key")));
        }

        clear_win(topwin);
        clear_win(win);
        clear_win(bottomwin);

        /* FREE CURRENT PART */
        free_part(current_part);
        current_part = NULL;

        /* FREE TEXT */
        for (temp = text; *temp; ++temp)
        {
            free(*temp);
            *temp = NULL;
        }

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

    free(title);
    title = NULL;

    free(date);
    date = NULL;

    destroy_win(topwin);
    destroy_win(win);
    destroy_win(bottomwin);
}

void write_text(char **story, WINDOW *win, int y_max, int x_max, int speed_0, int speed_1, char *name)
{
    int number_line = 0;
    int padding_top = (y_max / 2 - (y_max / 10)) / 2;
    int wait_time = speed_0;
    bool skip = false;
    size_t i, j;
    /* NAME VARs */
    unsigned int k = 0;
    unsigned int wrote_name;
    unsigned int n = 0;

    while (name[n + 1])
        n++;

    while (story[number_line])
        number_line++;

    padding_top -= number_line / 2;

    if (number_line % 2 != 0)
        padding_top--;

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
                        mvwprintw(win, j + padding_top, 2 + i + k + (n * wrote_name), "%c", name[k]);
                        fflush(stdout);
                        usleep(wait_time);
                        k++;
                    }
                    k = 0;
                    wrote_name++;
                }

                if (!wrote_name)
                    mvwprintw(win, j + padding_top, 2 + i, "%c", story[j][i]);
                else
                {
                    if (story[j][i] != '&')
                        mvwprintw(win, j + padding_top, 2 + i + (n * wrote_name), "%c", story[j][i]);
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

char *get_part_date(part *dpart)
{
    char *date = NULL;
    char *day = NULL;
    char *month = NULL;
    char *year = NULL;

    if (dpart == NULL)
        return NULL;

    if (dpart->date == NULL)
        return NULL;

    /* GET ALL DATE DATA */
    day = malloc(sizeof(char) * 3);  /* maximum number of digits + 1 */
    year = malloc(sizeof(char) * 5); /* maximum number of digits + 1 */

    sprintf(day, "%d", json_object_get_int(json_object_object_get(dpart->date, "day")));
    month = strdup(json_object_get_string(json_object_object_get(dpart->date, "month")));
    sprintf(year, "%d", json_object_get_int(json_object_object_get(dpart->date, "year")));

    /* PUT ALL DATE DATA TO DATE CHAR* */
    date = malloc(sizeof(char) * (strlen(day) + strlen(month) + strlen(year)) + 1 + 1 + 1);

    strcpy(date, day);
    strcat(date, " ");
    strcat(date, month);
    strcat(date, " ");
    strcat(date, year);

    /* FREEs */
    free(day);
    day = NULL;

    free(month);
    month = NULL;

    free(year);
    year = NULL;

    return date;
}
