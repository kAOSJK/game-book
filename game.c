#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses/ncurses.h>
#include "data/libs/lib.h"

#define DEFAULT_TEXT_SPEED_0 50000;
#define DEFAULT_TEXT_SPEED_1 10000;

void play_game(int, int, char*);
void display_title(WINDOW*, int, int, char*);

int main()
{
    /* JSON VARs */
    FILE *fp;
	char* buffer;
    size_t size;
    
    int menu_answer;
    int y_max, x_max;
    WINDOW* win;

    /* READ AND GET JSON DATA */
	fp = fopen("data/data.json", "r");

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = (char*)malloc(sizeof(char) * size + 1);
	fread(buffer, size, 1, fp);
	fclose(fp);

    initscr();
    getmaxyx(stdscr, y_max, x_max);
    noecho();
    cbreak();
    curs_set(0);

    menu_answer = display_menu(y_max, x_max, buffer);
    
    if (menu_answer == 0)
    {
        play_game(y_max, x_max, buffer);
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
    WINDOW* titlewin;
    WINDOW* agilitywin;
    bool succeed_agility;
    char** chapters;
    char*** parts;
    int story_length;
    int* chapter_length;
    int speed_0, speed_1;
    bool show_debug;
    unsigned int i, j;   

    /* GET GAME DATA */
    show_debug = get_json_data_boolean("show_debugger", buffer); /* boolean that show enable debug mode on true */
    speed_0 = get_json_data_int("text_speed_0", buffer); /* default text speed */
    if (!speed_0) speed_0 = DEFAULT_TEXT_SPEED_0; /* if speed_0 is NULL, load static default text speed 0 */
    speed_1 = get_json_data_int("text_speed_1", buffer); /* skipped text speed */
    if (!speed_1) speed_1 = DEFAULT_TEXT_SPEED_1; /* if speed_1 is NULL, load static default text speed 1 */

    story_length = get_json_options_length("story", buffer);
    chapters = (char**)malloc(story_length * sizeof(char*));
    for (i = 0; i < story_length; i++)
        chapters[i] = (char*)malloc(10 * sizeof(char));
    get_json_array_data(chapters, "story", buffer);

    parts = (char***)malloc(story_length * sizeof(char**));
    chapter_length = (int*)malloc(story_length * sizeof(int));

    for (j = 0; j < story_length; j++)
    {
        chapter_length[j] = get_json_options_length(chapters[j], buffer);

        if (chapter_length[j] != 0)
        {
            parts[j] = (char**)malloc(chapter_length[j] * sizeof(char*));
            for (i = 0; i < chapter_length[j]; i++)
                parts[j][i] = (char*)malloc(10 * sizeof(char));
            get_json_array_data(parts[j], chapters[j], buffer);

            display_title(titlewin, y_max, x_max, chapters[j]);            

            begin_story(y_max, x_max, speed_0, speed_1, chapter_length[j], parts[j], show_debug, buffer);

            /*
            if (get_json_data(parts[j][chapter_length[j] - 1], buffer)[strlen(get_json_data(parts[j][chapter_length[j] - 1], buffer)) - 1] == '*')
            {
                agilitywin = create_newwin(6, x_max - 12, y_max - 8, 5);
                box(agilitywin, 0 , 0);
                succeed_agility = agility(y_max, x_max, 7);
                while (1)
                {
                    if (succeed_agility)
                        mvwprintw(agilitywin, 1, 1, "You have passed the agility test: %c", get_json_data(parts[j][chapter_length[j] - 1], buffer)[strlen(get_json_data(parts[j][chapter_length[j] - 1], buffer)) - 1]);
                    else
                        mvwprintw(agilitywin, 1, 1, "You failed the agility test: %c", get_json_data(parts[j][chapter_length[j] - 1], buffer)[strlen(get_json_data(parts[j][chapter_length[j] - 1], buffer)) - 1]);
                    wrefresh(agilitywin);
                    if (wgetch(agilitywin) == 10) break;                             
                }
                destroy_win(agilitywin);
            }*/
        }
    }
}

void display_title(WINDOW* win, int y_max, int x_max, char* title)
{
    char* upper_title;
    unsigned int i;

    upper_title = (char*)malloc(sizeof(char) * strlen(title));

    for (i = 0; i < strlen(title); i++)
    {
        if (title[i] >= 'a' && title[i] <= 'z')
            upper_title[i] = title[i] - 32;
        else
            upper_title[i] = title[i];
    }

    win = create_newwin(3, strlen(title) + 4, y_max / 2 - 1.5, x_max / 2 - (strlen(title) / 2));
    box(win, 0 , 0);
    while (1)
    {
        mvwprintw(win, 1, 2, upper_title);
        wrefresh(win);
        if (wgetch(win) == 10) break;
    }
    destroy_win(win);
    free(upper_title);
}