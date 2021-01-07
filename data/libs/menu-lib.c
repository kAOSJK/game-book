#include "lib.h"

#define DRAW_HEIGHT (9)
#define DRAW_WIDTH (33)

int display_menu(int y_max, int x_max, char *language, char *buffer)
{
    char *menu_key = NULL;
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
    char **choices;
    int options_length;
    int choice;
    int i;
    int highlight = 0;

    /* GET LANGUAGE */
    if (strcmp(language, "English") == 0)
        menu_key = strdup("menu");
    else if (strcmp(language, "Francais") == 0)
        menu_key = strdup("menu_fr");
    else if (strcmp(language, "Wong jawa") == 0)
        menu_key = strdup("menu_jv");
    else
    {
        fprintf(stderr, "error: unknown language\n");
        return 4;
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
    /*
    printw("\ny_max: %d\n x_max: %d\n", y_max, x_max);
    refresh();
    getchar();*/
    menuwin = create_newwin(height, width, y_max / 2 - (height / 2), x_max / 2 - (width / 2));

    /* ACTIVATE ARROW KEYS */
    keypad(menuwin, true);

    options_length = get_json_options_length(menu_key, buffer);
    choices = malloc(sizeof(char *) * options_length);

    /* GET JSON FILE DATA */
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
