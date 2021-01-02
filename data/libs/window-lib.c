#include <string.h>
#include "lib.h"

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);
    return local_win;
}

WINDOW *create_window_var(int y_max, int x_max, int position)
{
    WINDOW *local_win;
    unsigned int height = 3;
    unsigned int width = 13;
    unsigned int i;
    unsigned int start_x;

    if (position == 0)
        start_x = 1;
    else if (position == 1)
        start_x = (x_max / 2) - (width / 2);
    else if (position == 2)
        start_x = x_max - (width + 1);

    if (position == 1)
        width++;

    local_win = newwin(height, width, 1, start_x);

    return local_win;
}

void refresh_window_var(WINDOW *local_win, int value, char *title)
{
    unsigned int height = 3;
    unsigned int width = 13;
    unsigned int i;

    box(local_win, 0, 0);
    wrefresh(local_win);

    mvwprintw(local_win, 0, (width / 2) - (strlen(title) / 2), title);
    for (i = 0; i < value; i++)
        mvwprintw(local_win, 1, i + 1, "=");

    wrefresh(local_win);
}

void destroy_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    wclear(local_win);
    delwin(local_win);
    clear();
    refresh();
}

void clear_win(WINDOW *local_win)
{
    werase(local_win);
    box(local_win, 0, 0);
    wrefresh(local_win);
}
