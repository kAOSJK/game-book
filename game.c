#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses/ncurses.h>
#include "lib.h"

int main()
{
    int y_max, x_max;
    
    initscr();
    getmaxyx(stdscr, y_max, x_max);
    noecho();
    cbreak();

    display_menu(y_max, x_max);

    endwin();

    return 0;
}