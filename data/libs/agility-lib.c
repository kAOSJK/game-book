#include <string.h>
#include <stdlib.h>
#include <ncurses/ncurses.h>
#include <unistd.h>
#include <time.h>
#include "lib.h"

int get_key(char input);
void refresh_qte(WINDOW* win, int size, int validated, char* qte);
char* randomize_qte(int size);

bool agility(int y_max, int x_max, int size)
{
    /* SCREEN VARs */
    WINDOW* menuwin;
    WINDOW* loadwin;

    /* DATA VARs */
    int input;
    int j = 0;
    int highlight = 0;
    int validated = 0;
    char* qte;
    char* str;

    str = (char*)malloc(sizeof(char) * 37);

    /* CREATE A WINDOW FOR OUR INPUT */
    menuwin = create_newwin(3, 36, y_max / 2 - 1.5, x_max / 2 - 9);
    wborder(menuwin, '|', '|', '-', '-', '+', '+', '+', '+');
    
    loadwin = create_newwin(3, 36, y_max / 2 - 5.5 , x_max / 2 - 9);
    box(loadwin, 0 , 0);

    keypad(menuwin, true); /* Activate arrow keys */

    qte = (char*)malloc((size + 1) * sizeof(char));
    qte = randomize_qte(size);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);

    refresh_qte(menuwin, size, validated, qte);
    while (1)
    {
        if (validated <= size)
        {
            nodelay(menuwin, true); /* non blocking getch */
            input = wgetch(menuwin); /* player's getch */
            
            if (input == get_key(qte[validated])) /* player success one qte's letter */
            {
                highlight++;
                validated++;
                refresh_qte(menuwin, size, validated, qte);
            }
            else if (input != -1) break; /* player failed */

            if (j < 34) /* loading bar */
            {
                str[j] = '=';
                mvwprintw(loadwin, 1, 1 + j, "%c", str[j]);
                wrefresh(loadwin);
                fflush(stdout);
                usleep(100000);
                j++;
            } else break;

            if (validated == size) break; /* successed qte */
        }
    }

    /* Delete windows */
    destroy_win(menuwin);
    destroy_win(loadwin);
    clear();
    refresh();

    /* FINAL MESSAGE */
    if (validated == size)
        return true;
    else
        return false;
}

void refresh_qte(WINDOW* win, int size, int validated, char* qte)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (i < validated)
            wattron(win, COLOR_PAIR(1));

        mvwprintw(win, 1, (i * 2) + 2, "%c", qte[i]);
        wattroff(win, COLOR_PAIR(1));
    }
}

char* randomize_qte(int size)
{
    int n;
    int i;
    char* randomized;
    randomized = (char*)malloc(sizeof(char) * size);

    srand(time(NULL));
    for (i = 0; i < size; i++)
    {
        n = rand() % 4; /* randomize n between 0 and 4 */
        if (n == 0) randomized[i] = 'Z';
        else if (n == 1) randomized[i] = 'Q';
        else if (n == 2) randomized[i] = 'S';
        else randomized[i] = 'D';
    }

    return randomized;
}

int get_key(char input)
{
    int answer = 999999999;

    if (input == 'Z')
        answer = 122;
    else if (input == 'D')
        answer = 100;
    else if (input == 'S')
        answer = 115;
    else if (input == 'Q')
        answer = 113;

    return answer;
}
