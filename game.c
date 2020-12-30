#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "data/libs/lib.h"

#define DEFAULT_TEXT_SPEED_0 (50000)
#define DEFAULT_TEXT_SPEED_1 (10000)
#define DEFAULT_MAX_NAME_SIZE (10)

void play_menu(int y_max, int x_max, char *language);
void play_game(int, int, char *, char *, char *);
void play_agility_game(int, int);
void display_title(int, int, array_list *, unsigned int);
void ask_new_name(int, int, char *, size_t);
char *int_to_word(int);
void update_json(char *temp, FILE *out);

int main()
{
    char *language = NULL;
    char *buffer = NULL;
    int y_max, x_max;
    FILE *fp;

    /* TODO: responsive
    Responsive : The game adapts itself to the size of the terminal */

    buffer = open_file(fp, "data/data.json", "r");

    if (buffer == NULL)
        return 0;

    language = get_json_object_string("default_language", buffer);

    /* START NCURSES */
    initscr();
    getmaxyx(stdscr, y_max, x_max);
    noecho();
    cbreak();
    curs_set(0);

    /*
    print_credits();
    getchar();*/

    /*
    printw("%s\n", language);
    refresh();
    getchar();*/

    play_menu(y_max, x_max, language);

    endwin();

    free(buffer);
    buffer = NULL;

    return 0;
}

void play_menu(int y_max, int x_max, char *language)
{
    int menu_answer;
    char *languages_answer = NULL;
    /* JSON VARs */
    char *usr_buffer = NULL;
    char *buffer = NULL;
    FILE *user;
    FILE *fp;
    /* NAME VARs */
    char *name = NULL;
    int max_name_size;

    /* READ AND GET JSON DATA */
    user = fopen("data/user.json", "r");

    buffer = open_file(fp, "data/data.json", "r");

    /*
    printw("current language is: %s", language);
    refresh();
    getchar();*/

    /* DISPLAY THE GAME MENU */
    menu_answer = display_menu(y_max, x_max, language, buffer);

    /* TODO: choose language
    Polyglot : The game supports at least one language other than english */

    /* MENU PLAYER CHOICE */
    if (menu_answer == 0)
    {
        if (user == NULL) /* means that the user play for the first time */
            create_player_json_data();

        usr_buffer = open_file(user, "data/user.json", "r");

        name = get_json_object_string("name", usr_buffer);
        if (!name || *name == 0)
        {
            max_name_size = get_json_data_int("max_name_size", buffer);
            if (!max_name_size)
                max_name_size = DEFAULT_MAX_NAME_SIZE;
            name = malloc(max_name_size);

            ask_new_name(y_max, x_max, name, max_name_size);
            set_json_object_string("name", name, user, usr_buffer);
            set_json_object_int("agility", 0, user, usr_buffer);
            set_json_object_int("mental", 0, user, usr_buffer);
            set_json_object_int("trust", 0, user, usr_buffer);
        }

        play_game(y_max, x_max, buffer, usr_buffer, name);
    }
    else if (menu_answer == 1)
    {
        play_agility_game(y_max, x_max);
    }
    else if (menu_answer == 2)
    {
        /* peut provoquer des leaks bg */
        languages_answer = display_languages(y_max, x_max, language, buffer);

        if (languages_answer != NULL)
        {
            /*
            printw("new language incoming is: %s", languages_answer);
            refresh();
            getchar();*/
            play_menu(y_max, x_max, languages_answer);
        }
    }

    fclose(user);

    /* FREES */
    free(buffer);
    buffer = NULL;

    free(usr_buffer);
    usr_buffer = NULL;

    free(name);
    name = NULL;

    free(language);
    language = NULL;
}

void play_game(int y_max, int x_max, char *buffer, char *usr_buffer, char *name)
{
    /* STORY VARs */
    struct json_object *parsed_json;
    struct array_list *parsed_story;
    chapter **story;
    /* OTHER VARs */
    WINDOW *titlewin;
    char ***parts;
    char **chapters;
    unsigned int *chapter_length;
    unsigned int speed_0, speed_1;
    unsigned int story_length;
    unsigned int i, j;

    /* GET GAME SETTINGS DATA */
    speed_0 = get_json_data_int("text_speed_0", buffer); /* default text speed */
    if (!speed_0)
        speed_0 = DEFAULT_TEXT_SPEED_0;                  /* if speed_0 is NULL, load static default text speed 0 */
    speed_1 = get_json_data_int("text_speed_1", buffer); /* skipped text speed */
    if (!speed_1)
        speed_1 = DEFAULT_TEXT_SPEED_1; /* if speed_1 is NULL, load static default text speed 1 */

    /* GET GAME DATA */
    parsed_json = json_tokener_parse(buffer);
    parsed_story = json_object_get_array(json_object_object_get(parsed_json, "story"));

    /* STORE DATA */
    story = get_story_data(parsed_story, buffer);

    /*
    add_mental_value(3, user, usr_buffer);
    add_agility_value(4, user, usr_buffer);
    add_trust_value(10, user, usr_buffer);
    */

    for (i = 0; story[i]; i++)
    {
        display_title(y_max, x_max, parsed_story, i);

        begin_chapter(y_max, x_max, speed_0, speed_1, story[i], i, parsed_story, name, buffer, usr_buffer);
    }

    free_story_data(story);
    int res = json_object_put(parsed_json);

    /*
    printf("\nFREE parsed_json result (1 if success):%d\n", res);
    getchar();*/
}

void display_title(int y_max, int x_max, array_list *story, unsigned int chapter_index)
{
    WINDOW *win;
    char *title = get_array_idx_key((char *)json_object_to_json_string(array_list_get_idx(story, chapter_index)));
    unsigned int i;

    win = create_newwin(3, strlen(title) + 4, y_max / 2 - 1.5, x_max / 2 - (strlen(title) / 2));
    while (1)
    {
        /* PRINT CAPITALIZED TITLE */
        for (i = 0; title[i]; i++)
            mvwprintw(win, 1, 2 + i, "%c", (IS_LOWER_CASE(title[i]) ? title[i] - 32 : title[i]));

        wrefresh(win);
        if (wgetch(win) == 10)
            break;
    }

    free(title);
    title = NULL;

    destroy_win(win);
}

void ask_new_name(int y_max, int x_max, char *name, size_t max_size)
{
    WINDOW *win;
    char *tmp = NULL;
    char ask[17] = "Enter your name:";
    char n[25];
    unsigned int i, j = 0;
    unsigned int k = 0;
    int c;

    curs_set(1);
    win = create_newwin(7, x_max - 6, y_max - 8, 3);
    box(win, 0, 0);

    for (i = 0; ask[i]; i++)
    {
        mvwprintw(win, 1, 2 + i, "%c", ask[i]);
        fflush(stdout);
        usleep(50000);
        wrefresh(win);
    }

    while (j < (unsigned int)max_size)
    {
        c = wgetch(win);
        if (c == 10)
            break;
        else if (c == 8)
        {
            n[j] = 0;
            j--;
            n[j] = 0;
            j--;
        }
        else
            n[j] = c;

        clear_win(win);
        mvwprintw(win, 1, 2, ask);
        for (k = 0; n[k] != 0; k++)
            mvwprintw(win, 1, 2 + i + k + 1, "%c", n[k]);
        wrefresh(win);
        j++;
    }

    n[j] = 0;

    tmp = realloc(name, strlen(n));
    if (tmp != NULL)
        strcpy(name, n);

    curs_set(0);
    destroy_win(win);
}

void play_agility_game(int y_max, int x_max)
{
    WINDOW *win;
    unsigned int i = 1;
    unsigned int score = 0;

    win = create_newwin(7, x_max - 6, y_max - 8, 3);
    while (i <= 10)
    {
        if (agility(y_max, x_max, 6))
        {
            while (1)
            {
                box(win, 0, 0);
                mvwprintw(win, 1, 2, "You have passed the %s agility test", int_to_word(i));
                if (wgetch(win) == 10)
                    break;
            }
            score++;
        }
        /* means that it's a failure */
        else
        {
            while (1)
            {
                box(win, 0, 0);
                mvwprintw(win, 1, 2, "You failed the %s agility test", int_to_word(i));
                if (wgetch(win) == 10)
                    break;
            }
        }
        wclear(win);
        wrefresh(win);
        i++;
    }

    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Your score is : %d out of 10", score);
    while (1)
    {
        if (wgetch(win) == 10)
            break;
    }

    destroy_win(win);
}
