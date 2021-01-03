#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "lib.h"

int get_key(char input);
void refresh_qte(WINDOW *win, int size, int validated, char *qte, bool failed);
char *randomize_qte(int size);

bool agility(int y_max, int x_max, int size, int speed)
{
    /* WINDOWS VARs */
    WINDOW *menuwin;
    WINDOW *loadwin;
    int height = 3;
    int width = 36;
    /* DATA VARs */
    char *loading_str; /* loading bar that contains this advancment of the loading */
    char *qte;         /* this is the string that contains the qte */
    int validated = 0;
    int input;
    int j = 0;

    loading_str = (char *)malloc(sizeof(char) * 37);

    /* CREATE A WINDOW FOR OUR INPUT */
    menuwin = create_newwin(height, width, y_max / 2 - (height / 2), x_max / 2 - (width / 2));
    wborder(menuwin, '|', '|', '-', '-', '+', '+', '+', '+');

    loadwin = create_newwin(height, width, y_max / 2 - height - 1, x_max / 2 - (width / 2));
    box(loadwin, 0, 0);

    keypad(menuwin, true); /* Activate arrow keys */

    qte = randomize_qte(size);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    refresh_qte(menuwin, size, validated, qte, false);
    while (1)
    {
        if (validated <= size)
        {
            nodelay(menuwin, true);  /* non blocking getch */
            input = wgetch(menuwin); /* player's getch */

            if (input == get_key(qte[validated])) /* player success one qte's letter */
            {
                validated++;
                refresh_qte(menuwin, size, validated, qte, false);
            }
            else if (input != -1 && input != 10)
            {
                input = -1;
                refresh_qte(menuwin, size, validated, qte, true);

                wrefresh(menuwin);
                fflush(stdout);
                usleep(speed * 10);

                break; /* player failed */
            }

            if (j < 34) /* loading bar */
            {
                loading_str[j] = '=';
                mvwprintw(loadwin, 1, 1 + j, "%c", loading_str[j]);
                wrefresh(loadwin);
                fflush(stdout);
                usleep(speed);
                j++;
            }
            else
                break;

            if (validated == size)
                break; /* successed qte */
        }
    }

    /* Free */
    free(loading_str);
    loading_str = NULL;

    free(qte);
    qte = NULL;

    /* DELETE WINDOWS */
    destroy_win(menuwin);
    destroy_win(loadwin);

    /* FINAL RETURN */
    if (validated == size)
        return true;
    else
        return false;
}

void refresh_qte(WINDOW *win, int size, int validated, char *qte, bool failed)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (i < validated)
            wattron(win, COLOR_PAIR(1));

        /* PLAYER'S FAILURE CASE */
        if (failed)
        {
            if (i == validated)
                wattron(win, COLOR_PAIR(2));
        }

        mvwprintw(win, 1, (i * 2) + 2, "%c", qte[i]);
        wattroff(win, COLOR_PAIR(1));
    }
}

char *randomize_qte(int size)
{
    int n;
    int i;
    char *randomized;
    randomized = (char *)malloc(sizeof(char) * size);

    srand(time(NULL));
    for (i = 0; i < size; i++)
    {
        n = rand() % 4; /* randomize n between 0 and 4 */
        if (n == 0)
            randomized[i] = 'Z';
        else if (n == 1)
            randomized[i] = 'Q';
        else if (n == 2)
            randomized[i] = 'S';
        else
            randomized[i] = 'D';
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
