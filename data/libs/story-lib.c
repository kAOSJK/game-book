#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses/ncurses.h>
#include <unistd.h>
#include "lib.h"

#define CHAR_SEPARATOR "$"
#define CHAR_SEPARATOR_2 "/"

void begin_story(int y_max, int x_max, int speed_0, int speed_1, int story_length, char** parts, bool show_debug, char* buffer)
{
    int i, j;
    /* SCREEN VARs */
    WINDOW* win;
    WINDOW* debug;
    char** story = NULL;
    char **p = NULL;

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
        write_story(story, win, y_max, x_max, speed_0, speed_1, parts[i + 1], show_debug);
        clear_win(win);

        if (story != NULL)
	    {
            for (p = story; *p; ++p) puts(*p);
            for (p = story; *p; ++p) free(*p);
            free(story);
	    }
    }

    destroy_win(win);
    if (show_debug) destroy_win(debug);
}

void write_story(char** story, WINDOW* win, int y_max, int x_max, int speed_0, int speed_1, char* next, bool show_debug)
{
    size_t i, j;
    bool skip = false;
    bool print_stop = false;
    int wait_time = speed_0;
    /* AGILITY VARs */
    WINDOW* agilitywin;
    bool succeed_agility;
    bool need_agility;
    bool encounter_separator = false;
    char* win_dir = NULL;
    char* lose_dir = NULL;
    char* token = NULL;
    int index_agility;

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
                if (story[j][i] == '*')
                {
                    print_stop = true;
                    need_agility = true;
                    index_agility = i;
                }
                if (!print_stop)
                    mvwprintw(win, 1 + j, 2 + i, "%c", story[j][i]);
                wrefresh(win);
            }
        }
        nodelay(win, false);

        mvwprintw(win, 0, 1, "%c", story[j - 1][index_agility + 1]);
        wrefresh(win);

        if (wgetch(win) == 10) break;
    }

    /* AGILITY TEST COMING RIGHT AFTER */
    if (need_agility)
    {
        clear_win(win);
        if (story[j - 1][index_agility + 1] - 48 > 0 && story[j - 1][index_agility + 1] - 48 <= 9)
            succeed_agility = agility(y_max, x_max, story[j - 1][index_agility + 1] - 48);
        else
            succeed_agility = agility(y_max, x_max, 5);
        
        token = strtok(story[j - 1], CHAR_SEPARATOR_2);
        
        while (token)
		{
            if (!encounter_separator)
            {
                token = strtok(NULL, CHAR_SEPARATOR_2);
                win_dir = malloc(strlen(token) + 1);
                if (win_dir != NULL) strcpy(win_dir, token);
                encounter_separator = true;
            }
            else
            {
                lose_dir = malloc(strlen(token) + 1);
                if (lose_dir != NULL) strcpy(lose_dir, token);
            }
            token = strtok(NULL, CHAR_SEPARATOR_2);
		}

        while (1)
        {
            if (show_debug)
            {
                agilitywin = create_newwin(6, x_max - 12, y_max - 8, 5);
                box(agilitywin, 0 , 0);
            }

            if (succeed_agility)
            {
                if (show_debug) mvwprintw(agilitywin, 1, 2, "you win, jumping into key :%s", win_dir);
                if (next != NULL) strcpy(next, win_dir);
            }
            else
            {
                if (show_debug)
                    mvwprintw(agilitywin, 1, 2, "you lost, jumping into key :%s", lose_dir);
                if (next != NULL) strcpy(next, lose_dir);
            }

            if (show_debug)
            {
                wrefresh(agilitywin);
                if (wgetch(agilitywin) == 10) break;
            } else break;                             
        }

        free(win_dir);
        free(lose_dir);
        destroy_win(agilitywin);
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