#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "lib.h"

int display_menu(int y_max, int x_max, char *buffer)
{
    /* SCREEN VARs */
    WINDOW *menuwin;
    /* DATA VARs */
    char **choices;
    int options_length;
    int choice;
    int i;
    int highlight = 0;

    /* CREATE MENU WINDOW */
    getmaxyx(stdscr, y_max, x_max);
    menuwin = create_newwin(6, 12, y_max / 2 - 3, x_max / 2 - 6);

    /* ACTIVATE ARROW KEYS */
    keypad(menuwin, true);

    options_length = get_json_options_length("menu", buffer);
    choices = malloc(sizeof(char *) * options_length);

    /* GET JSON FILE DATA */
    get_json_array_data(choices, "menu", buffer);

    while (1)
    {
        for (i = 0; i < options_length; i++)
        {
            if (i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i + 1, 2, "%s", choices[i]);
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
        free(choices[i]);
    free(choices);

    destroy_win(menuwin);

    return highlight;
}
