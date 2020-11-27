#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses/ncurses.h>
#include "lib.h"

int main()
{
    int menu_answer;
    int y_max, x_max;
    WINDOW* win;

    initscr();
    getmaxyx(stdscr, y_max, x_max);
    noecho();
    cbreak();

    if (display_menu(y_max, x_max) == 1)
    {
        win = create_newwin(6, x_max - 12, y_max - 8, 5);
        box(win, 0 , 0);
        if (agility(y_max, x_max, 7))
            mvwprintw(win, 1, 1, "You have passed the agility test");
        else
            mvwprintw(win, 1, 1, "You failed the agility test");
        wrefresh(win);
        getchar();
    }
    /*
    menu_answer = display_menu(y_max, x_max);
    if (menu_answer == 0) /* player choosed play
    { 
        win = create_newwin(6, x_max - 12, y_max - 8, 5);
        if (agility(y_max, x_max, 10))
            mvwprintw(win, 1, 1, "win");
        else
            mvwprintw(win, 1, 1, "lose");
        wrefresh(win);
    }
    else if (menu_answer == 1) /* player choosed settings
    {
        win = create_newwin(6, x_max - 12, y_max - 8, 5);
        mvwprintw(win, 1, 1, "options");
        wrefresh(win);
    }
    else if (menu_answer == 2) /* player choosed quit
    {
        win = create_newwin(6, x_max - 12, y_max - 8, 5);
        mvwprintw(win, 1, 1, "quit");
        wrefresh(win);
    }*/

    endwin();

    return 0;
}