#include <ncurses/ncurses.h>
#include "lib.h"

WINDOW* create_newwin(int height, int width, int starty, int startx)
{ 
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wrefresh(local_win);
    return local_win;
}

void destroy_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(local_win);
    wclear(local_win);
    delwin(local_win);
    refresh();
}

void clear_win(WINDOW *local_win)
{
    werase(local_win);
    box(local_win, 0 , 0);
    wrefresh(local_win);
}
