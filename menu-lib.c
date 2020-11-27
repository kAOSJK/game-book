#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses/ncurses.h>
#include <unistd.h>
#include "lib.h"

#define CHAR_SEPARATOR "$"

char** sentence_separator(char* str);
void write_story(char** story, WINDOW* win, int speed_0, int speed_1);
void destroy_story(char** story);

int display_menu(int y_max, int x_max)
{
    /* JSON VARs */
    FILE *fp;
	char buffer[1024];
    /* SCREEN VARs */
    WINDOW* menuwin;
    WINDOW* win;
    WINDOW* debug;
    /* DATA VARs */
    char** choices;
    int options_length;
    int choice;
    int i;
    int highlight = 0;
    /* STORY VARs */
    char** parts;
    int speed_0, speed_1;
    int story_length;

    /* READ AND GET JSON DATA */
	fp = fopen("menu.json", "r");
	fread(buffer, 1024, 1, fp);
	fclose(fp);
    speed_0 = get_json_data_int("text_speed_0", buffer);
    speed_1 = get_json_data_int("text_speed_1", buffer);

    /* CREATE MENU WINDOW */
    getmaxyx(stdscr, y_max, x_max);
    menuwin = create_newwin(6, 12, y_max / 2 - 3, x_max / 2 - 6);

    /* ACTIVATE ARROW KEYS */
    keypad(menuwin, true); 

    options_length = get_json_options_length("options", buffer);
    choices = malloc(options_length * sizeof(char*));

    for (i = 0; i < options_length; i++)
        choices[i] = malloc(10 * sizeof(char));

    /* GET JSON FILE DATA */
    get_json_array_data(choices, "options", buffer);

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
            case KEY_DOWN: highlight == options_length - 1 ? highlight = 0 : highlight++; break;
            case KEY_UP:   highlight == 0 ? highlight = options_length - 1 : highlight--; break;
            case 'p':      getchar(); break;
            default:       break;
        }
        if (choice == 10) break;
    }

    /* CREATE A WINDOW FOR OUR INPUT */
    destroy_win(menuwin);
    refresh();
    win = create_newwin(6, x_max - 12, y_max - 8, 5);

    /* FINAL MESSAGE */
    if (highlight == 0) /* It mean that it's play */
    {
        story_length = get_json_options_length("introduction", buffer);
        parts = malloc(story_length * sizeof(char*));

        for (i = 0; i < story_length; i++)
            parts[i] = malloc(10 * sizeof(char));

        parts = get_json_array_data(parts, "introduction", buffer);

        if (get_json_data_boolean("show_debugger", buffer))
        {
            debug = create_newwin(12, 50, 2, 6);
            mvwprintw(debug, 1, 22, "Debugger");
            mvwprintw(debug, 3, 2, "Charged the JSON following key:");
            wrefresh(debug);
        }

        for (i = 0; i < story_length; i++)
        {
            /* DEBUG PART */
            if (debug != NULL)
            {
                mvwprintw(debug, i + 5, 2, "key \"%s\" at index:%d", parts[i], i);
                mvwprintw(debug, i + 5, 40, "i: %d", i);
                wrefresh(debug);
            }
            
            /* WRITE STORY PART */
            write_story(sentence_separator(get_json_data(parts[i], buffer)), win, speed_0, speed_1);
            clear_win(win);
        }

        if (get_json_data_boolean("show_debugger", buffer))
        {
            mvwprintw(debug, story_length + 6, 2, "I've charged all JSON key founded");
            wrefresh(debug);
        }
    }
    return highlight;
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
        tab_str[i] = (char*)malloc(sizeof(char) * 125);
        tab_str[i] = token;
        token = strtok(NULL, CHAR_SEPARATOR);
        i++;
    }

    return tab_str;
}
