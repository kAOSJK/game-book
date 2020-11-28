#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses/ncurses.h>
#include "data/libs/lib.h"

void play_game(int y_max, int x_max, char* buffer);

int main()
{
    /* JSON VARs */
    FILE *fp;
	char buffer[1024];

    int menu_answer;
    int y_max, x_max;
    WINDOW* win;

    /* READ AND GET JSON DATA */
	fp = fopen("data/menu.json", "r");
	fread(buffer, 1024, 1, fp);
	fclose(fp);

    initscr();
    getmaxyx(stdscr, y_max, x_max);
    noecho();
    cbreak();

    menu_answer = display_menu(y_max, x_max, buffer);
    
    if (menu_answer == 0)
    {
        play_game(y_max, x_max, buffer);

        win = create_newwin(6, x_max - 12, y_max - 8, 5);
        box(win, 0 , 0);
        if (agility(y_max, x_max, 7))
            mvwprintw(win, 1, 1, "You have passed the agility test");
        else
            mvwprintw(win, 1, 1, "You failed the agility test");
        wrefresh(win);
        getchar();
    }
    else if (menu_answer == 1)
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

    endwin();

    return 0;
}

void play_game(int y_max, int x_max, char* buffer)
{
    char** parts;
    int speed_0, speed_1;
    int story_length;
    bool show_debug;
    size_t i;   

    /* GET GAME DATA */
    show_debug = get_json_data_boolean("show_debugger", buffer);
    speed_0 = get_json_data_int("text_speed_0", buffer);
    speed_1 = get_json_data_int("text_speed_1", buffer);
    story_length = get_json_options_length("introduction", buffer);
    parts = malloc(story_length * sizeof(char*));
    for (i = 0; i < story_length; i++)
        parts[i] = malloc(10 * sizeof(char));
    parts = get_json_array_data(parts, "introduction", buffer);

    begin_story(y_max, x_max, speed_0, speed_1, story_length, parts, show_debug, buffer);
}