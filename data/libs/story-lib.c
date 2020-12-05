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
    int i, j;
    /* SCREEN VARs */
    WINDOW* win;
    WINDOW* debug;
    char** story = NULL;
    char **p = NULL;
    /*
    WINDOW* agilitywin;
    bool succeed_agility;*/

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

    for (i = 0, j = 0; i < story_length; i++)
    {
        /* DEBUG PART */
        if (show_debug)
        {
            if (j == 6) j = 0;
            mvwprintw(debug, j + 5, 2, "key \"%s\" at index:%d", parts[i], i);
            mvwprintw(debug, j + 5, 40, "i: %d", i);
            wrefresh(debug);
            j++;
        }
        
        /* WRITE STORY PART */
        story = sentence_separator(get_json_data(parts[i], buffer));
        write_story(story, win, speed_0, speed_1);
        clear_win(win);

        if (story != NULL)
	    {
            for (p = story; *p; ++p) puts(*p);
            for (p = story; *p; ++p) free(*p);
            free(story);
	    }
    }

    /* new implémentation */
    /*
    if (get_json_data(parts[story_length - 1], buffer)[strlen(get_json_data(parts[story_length - 1], buffer)) - 1] == '*')
    {
        agilitywin = create_newwin(6, x_max - 12, y_max - 8, 5);
        box(agilitywin, 0 , 0);
        succeed_agility = agility(y_max, x_max, 7);
        while (1)
        {
            if (succeed_agility)
                mvwprintw(agilitywin, 1, 1, "You have passed the agility test: %c", get_json_data(parts[story_length - 1], buffer)[strlen(get_json_data(parts[story_length - 1], buffer)) - 1]);
            else
                mvwprintw(agilitywin, 1, 1, "You failed the agility test: %c", get_json_data(parts[story_length - 1], buffer)[strlen(get_json_data(parts[story_length - 1], buffer)) - 1]);
            wrefresh(agilitywin);
            if (wgetch(agilitywin) == 10) break;                             
        }
        destroy_win(agilitywin);
    }*/

    destroy_win(win);
    if (show_debug) destroy_win(debug);
}

void write_story(char** story, WINDOW* win, int speed_0, int speed_1)
{
    size_t i, j;
    bool skip = false;
    bool print_stop = false;
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
                if (story[j][i] == '*') print_stop = true;
                if (!print_stop)
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
	char** array = malloc(sizeof(char*));
    char** tmp = NULL;
    char* token = NULL;
    size_t n;

	if (array)
	{
        n = 1;
		token = strtok(str, CHAR_SEPARATOR);
		while (token)
		{
			tmp = realloc(array, (n + 1) * sizeof(char*));
			if (tmp == NULL) break;

			array = tmp;
			++n;

			array[n - 2] = malloc(strlen(token) + 1);
			if (array[n - 2] != NULL) strcpy(array[n - 2], token);
			token = strtok(NULL, CHAR_SEPARATOR);
		}

		array[n - 1] = NULL;
	}

	return array;
}