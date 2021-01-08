#include "lib.h"

#define DRAW_HEIGHT (9)
#define DRAW_WIDTH (33)

int display_menu(int y_max, int x_max, char **language_ptr, char *buffer)
{
    char *language = *language_ptr;
    char *menu_key = NULL;
    /* WINDOWS VARs */
    WINDOW *menuwin;
    WINDOW *drawin;
    WINDOW *drawin_2;
    int height = 13;
    int width = 24;
    /* STORY LANGUAGES DATA VARs */
    char **languages_code = NULL;
    char **languages = NULL;
    char *menu_language = NULL;
    char *code = NULL;
    unsigned int languages_length = 0;
    unsigned int languages_code_length = 0;
    unsigned int index = 0;
    /* DRAWINGS VARs */
    char *draw_0 = NULL;
    char *draw_1 = NULL;
    /* DATA VARs */
    char **choices;
    int options_length;
    int choice;
    int i;
    int highlight = 0;

    /* GET LANGUAGE MIN LENGTH, MALLOC IT, AND GET IT */
    languages_code_length = get_json_options_length("languages_code", buffer);
    languages_code = malloc(sizeof(char *) * languages_code_length);
    get_json_array_data(languages_code, "languages_code", buffer);

    /* GET LANGUAGE LENGTH, MALLOC IT, AND GET IT */
    languages_length = get_json_options_length("languages", buffer);
    languages = malloc(sizeof(char *) * languages_length);
    get_json_array_data(languages, "languages", buffer);

    /* GET THE RIGHT INDEX */
    for (i = 0; i < languages_length; i++)
    {
        if (strcmp(languages[i], language) == 0)
        {
            index = i;
            break;
        }
    }

    /* GET RIGHT STORY LANGUAGE */
    code = strdup(languages_code[index]);

    menu_language = strdup("menu");
    if (strcmp(code, "") != 0)
    {
        menu_language = realloc(menu_language, sizeof(char) * strlen(menu_language) + strlen(code) + 1 + 1);
        strcat(menu_language, "_");
        strcat(menu_language, code);
    }

    /* GET MENU LENGTH DATA */
    menu_key = strdup(menu_language);
    options_length = get_json_options_length(menu_key, buffer);

    if (options_length == 0)
    {
        /* MESSAGE OF BAD CHOOSED LANGUAGE */
        clear();
        printw("\nTranslation of the choosen language is not finished, switching to the default language\n\nYou can change at any moment the default language in the data.json file");
        refresh();
        getchar();

        /* FREE LANGUAGE */
        free(menu_language);
        menu_language = NULL;

        free(menu_key);
        menu_key = NULL;

        /* LOAD DEFAULT LANGUAGE */
        menu_language = strdup("menu");
        if (strcmp(languages_code[0], "") != 0)
        {
            menu_language = realloc(menu_language, sizeof(char) * strlen(menu_language) + strlen(languages_code[0]) + 1 + 1);
            strcat(menu_language, "_");
            strcat(menu_language, languages_code[0]);
        }

        /* GET MENU LENGTH DATA */
        menu_key = strdup(menu_language);
        options_length = get_json_options_length(menu_key, buffer);

        if (options_length == 0)
        {
            clear();
            printw("\nTranslation of the choosen default language is not finished, switching to static default language");
            refresh();
            getchar();

            /* GET MENU LENGTH DATA */
            menu_key = strdup("menu");
            options_length = get_json_options_length(menu_key, buffer);
        }

        clear();
        refresh();
    }

    /* CREATE DRAWINGS WINDOWS */
    drawin = newwin(DRAW_HEIGHT, DRAW_WIDTH, y_max / 2 - (DRAW_HEIGHT / 2), x_max / 2 - (DRAW_WIDTH / 2) - DRAW_WIDTH + 5);
    wrefresh(drawin);

    drawin_2 = newwin(DRAW_HEIGHT, DRAW_WIDTH, y_max / 2 - (DRAW_HEIGHT / 2), x_max / 2 - (DRAW_WIDTH / 2) + DRAW_WIDTH - 5);
    wrefresh(drawin_2);

    /* OPEN DRAWINGS FILES */
    draw_0 = open_file("data/lion_3_0", "r");
    draw_1 = open_file("data/lion_3_1", "r");

    wprintw(drawin, draw_0);
    wrefresh(drawin);

    wprintw(drawin_2, draw_1);
    wrefresh(drawin_2);

    /* CREATE MENU WINDOW */
    getmaxyx(stdscr, y_max, x_max);
    menuwin = create_newwin(height, width, y_max / 2 - (height / 2), x_max / 2 - (width / 2));

    /* ACTIVATE ARROW KEYS */
    keypad(menuwin, true);

    /* GET MENU DATA */
    choices = malloc(sizeof(char *) * options_length);
    get_json_array_data(choices, menu_key, buffer);

    while (1)
    {
        for (i = 0; i < options_length; i++)
        {
            if (i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, (i * 2) + 1 + 1, (width / 2) - (strlen(choices[i]) / 2), "%s", choices[i]);
            wattroff(menuwin, A_REVERSE);
        }

        choice = wgetch(menuwin);

        switch (choice)
        {
        case KEY_DOWN:
            highlight == options_length - 1 ? highlight = 0 : highlight++;
            break;
        case KEY_UP:
            highlight == 0 ? highlight = options_length - 1 : highlight--;
            break;
        default:
            break;
        }

        if (choice == 10)
            break;
    }

    /* FREE LANGUAGE MENU VARs */
    for (i = 0; i < languages_code_length; i++)
    {
        free(languages_code[i]);
        languages_code[i] = NULL;
    }

    free(languages_code);
    languages_code = NULL;

    for (i = 0; i < languages_length; i++)
    {
        free(languages[i]);
        languages[i] = NULL;
    }

    free(languages);
    languages = NULL;

    free(code);
    code = NULL;

    free(menu_language);
    menu_language = NULL;

    /* OTHERS FREE */
    for (i = 0; i < options_length; i++)
    {
        free(choices[i]);
        choices[i] = NULL;
    }

    free(choices);
    choices = NULL;

    free(menu_key);
    menu_key = NULL;

    free(draw_0);
    draw_0 = NULL;

    free(draw_1);
    draw_1 = NULL;

    destroy_win(menuwin);
    destroy_win(drawin);
    destroy_win(drawin_2);

    return highlight;
}

char *display_languages(int y_max, int x_max, char *buffer)
{
    /* WINDOWS VARs */
    WINDOW *menuwin;
    WINDOW *drawin;
    WINDOW *drawin_2;
    int height = 13;
    int width = 24;
    /* DRAWINGS VARs */
    char *draw_0 = NULL;
    char *draw_1 = NULL;
    /* DATA VARs */
    char **choices = NULL;
    char *answer = NULL;
    int options_length;
    int choice;
    int i;
    int highlight = 0;

    /* CREATE DRAWINGS WINDOWS */
    drawin = newwin(DRAW_HEIGHT, DRAW_WIDTH, y_max / 2 - (DRAW_HEIGHT / 2), x_max / 2 - (DRAW_WIDTH / 2) - DRAW_WIDTH + 5);
    wrefresh(drawin);

    drawin_2 = newwin(DRAW_HEIGHT, DRAW_WIDTH, y_max / 2 - (DRAW_HEIGHT / 2), x_max / 2 - (DRAW_WIDTH / 2) + DRAW_WIDTH - 5);
    wrefresh(drawin_2);

    /* OPEN DRAWINGS FILES */
    draw_0 = open_file("data/lion_3_0", "r");
    draw_1 = open_file("data/lion_3_1", "r");

    wprintw(drawin, draw_0);
    wrefresh(drawin);

    wprintw(drawin_2, draw_1);
    wrefresh(drawin_2);

    /* CREATE MENU WINDOW */
    getmaxyx(stdscr, y_max, x_max);
    menuwin = create_newwin(height, width, y_max / 2 - (height / 2), x_max / 2 - (width / 2));

    /* ACTIVATE ARROW KEYS */
    keypad(menuwin, true);

    options_length = get_json_options_length("languages", buffer);
    choices = malloc(sizeof(char *) * options_length);

    /* GET JSON FILE DATA */
    get_json_array_data(choices, "languages", buffer);

    while (1)
    {
        for (i = 0; i < options_length; i++)
        {
            if (i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, (i * 2) + 1 + 1, (width / 2) - (strlen(choices[i]) / 2), "%s", choices[i]);
            wattroff(menuwin, A_REVERSE);
        }

        choice = wgetch(menuwin);

        switch (choice)
        {
        case KEY_DOWN:
            highlight == options_length - 1 ? highlight = 0 : highlight++;
            break;
        case KEY_UP:
            highlight == 0 ? highlight = options_length - 1 : highlight--;
            break;
        default:
            break;
        }

        if (choice == 10)
            break;
    }

    answer = strdup(choices[highlight]);

    for (i = 0; i < options_length; i++)
    {
        free(choices[i]);
        choices[i] = NULL;
    }

    free(choices);
    choices = NULL;

    free(draw_0);
    draw_0 = NULL;

    free(draw_1);
    draw_1 = NULL;

    destroy_win(menuwin);
    destroy_win(drawin);
    destroy_win(drawin_2);

    return answer;
}
