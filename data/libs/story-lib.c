#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses/ncurses.h>
#include <unistd.h>
#include "lib.h"

#define CHAR_SEPARATOR "$"

void begin_story(int y_max, int x_max, int speed_0, int speed_1, int story_length, char** parts, bool show_debug, char* buffer)
{
    int i;
    /* SCREEN VARs */
    WINDOW* win;
    WINDOW* debug;

    /* CREATE A WINDOW FOR OUR INPUT */
    win = create_newwin(6, x_max - 12, y_max - 8, 5);
    
    /* DEBUG PART */
    if (show_debug)
    {
        debug = create_newwin(12, 50, 2, 6);
        mvwprintw(debug, 1, 22, "Debugger");
        mvwprintw(debug, 3, 2, "Charged the JSON following key:");
        wrefresh(debug);
    }

    for (i = 0; i < story_length; i++)
    {
        /* DEBUG PART */
        if (show_debug)
        {
            mvwprintw(debug, i + 5, 2, "key \"%s\" at index:%d", parts[i], i);
            mvwprintw(debug, i + 5, 40, "i: %d", i);
            wrefresh(debug);
        }
        
        /* WRITE STORY PART */
        write_story(sentence_separator(get_json_data(parts[i], buffer)), win, speed_0, speed_1);
        clear_win(win);
    }

    /* DEBUG PART */
    if (show_debug)
    {
        mvwprintw(debug, story_length + 6, 2, "I've charged all JSON key founded");
        wrefresh(debug);
    }
}

void write_story(char** story, WINDOW* win, int speed_0, int speed_1)
{
    size_t i;
    size_t j;
    bool skip = false;
    int wait_time = speed_0;

    keypad(win, true);

    while (1)
    {
        for (j = 0; story[j]; j++)
        {
            for (i = 0; story[j][i]; i++)
            {
                if (!skip)
                {
                    nodelay(win, true);
                    if (wgetch(win) == 10)
                        wait_time = speed_1;
                }

                fflush(stdout);
                usleep(wait_time);
                mvwprintw(win, 1 + j, 2 + i, "%c", story[j][i]);
                wrefresh(win);
            }
        }
        nodelay(win, false);

        if (wgetch(win) == 10) break;
    }
}

char** sentence_separator(char* str)
{
    char** tab_str;
    char* token = strtok(str, CHAR_SEPARATOR);
    size_t i = 0;

    tab_str = (char**)malloc(sizeof(char*));

    while (token != NULL)
    {
        tab_str = realloc(tab_str, sizeof(char*) * (i + 1));
        tab_str[i] = (char*)malloc(sizeof(char) * (strlen(token) + 1));
        tab_str[i] = token;
        token = strtok(NULL, CHAR_SEPARATOR);
        i++;
    }

    return tab_str;
}