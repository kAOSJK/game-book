#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ncurses/ncurses.h>
#include "data/libs/lib.h"

#define DEFAULT_TEXT_SPEED_0 (50000)
#define DEFAULT_TEXT_SPEED_1 (10000)
#define DEFAULT_MAX_NAME_SIZE (10)

#define IS_LOWER_CASE(c) ((c >= 'A' && c <= 'Z'))

void play_game(int, int, char*, char*);
void play_agility_game(int, int);
void display_title(WINDOW*, int, int, const char*);
void ask_new_name(int, int, char*, size_t);
char* int_to_word(int);
void update_json(char* temp, FILE* out);

int main()
{
    WINDOW* win;
    int menu_answer;
    int y_max, x_max;
    /* JSON VARs */
    FILE *fp;
    FILE *out;
	char* buffer;
    char* temp = NULL;
    size_t size;
    /* NAME VARs */
    char* name = NULL;
    int max_name_size;

    /* READ AND GET JSON DATA */
	fp = fopen("data/data.json", "r");

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = malloc(size);
	fread(buffer, size, 1, fp);
    fclose(fp);

    /*
    get_json_object_data("player", buffer);
    if (change_json_object_data("player", 0, "blblbl", buffer) == 1)
        fputs(buffer, out);
    get_json_object_data("player", buffer);*/

    initscr();
    getmaxyx(stdscr, y_max, x_max);
    noecho();
    cbreak();  
    curs_set(0);

    menu_answer = display_menu(y_max, x_max, buffer);
    
    if (menu_answer == 0)
    {
        max_name_size = get_json_data_int("max_name_size", buffer);
        if (!max_name_size)
            max_name_size = DEFAULT_MAX_NAME_SIZE;
        name = malloc(max_name_size);

        ask_new_name(y_max, x_max, name, (size_t)max_name_size);
        
        /* UPDATE JSON FILE
        temp = replaceWord(buffer, "&", name);
        update_json(temp, out);*/

        change_json_object_data("player", 0, name, buffer);
        play_game(y_max, x_max, buffer, name);
    }
    else if (menu_answer == 1)
    {
        play_agility_game(y_max, x_max);
    }

    free(buffer);
    free(name);

    endwin();

    return 0;
}

void play_game(int y_max, int x_max, char* buffer, char* name)
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

            begin_story(y_max, x_max, speed_0, speed_1, chapter_length[j], parts[j], show_debug, buffer, name);
        }
    }
}

void display_title(WINDOW* win, int y_max, int x_max, const char* title)
{
    char* cpy = strdup(title);
    unsigned int i;

    win = create_newwin(3, strlen(title) + 4, y_max / 2 - 1.5, x_max / 2 - (strlen(title) / 2));
    while (1)
    {
        for (i = 0; cpy[i]; i++)
            mvwprintw(win, 1, 2 + i, "%c", (IS_LOWER_CASE(cpy[i]) ? cpy[i] : cpy[i] - 32));
        wrefresh(win);
        if (wgetch(win) == 10) break;
    }

    destroy_win(win);
    free(cpy);
}

void ask_new_name(int y_max, int x_max, char* name, size_t max_size)
{
    WINDOW* win;
    char* tmp = NULL;
    char ask[17] = "Enter your name:";
    char n[25];
    unsigned int i, j = 0;
    unsigned int k = 0;
    int c;

    curs_set(1);
    win = create_newwin(6, x_max - 12, y_max - 8, 5);
    box(win, 0 , 0);

    for (i = 0; ask[i]; i++)
    {
        mvwprintw(win, 1, 2 + i, "%c", ask[i]);
        fflush(stdout);
        usleep(50000);
        wrefresh(win);
    }

    while (j < (int)max_size)
    {
        c = wgetch(win);
        if (c == 10)
            break;
        else if (c == 8)
        {
            n[j] = 0;
            j--;
            n[j] = 0;
            j--;
        } else
            n[j] = c;
        
        clear_win(win);
        mvwprintw(win, 1, 2, ask);
        for (k = 0; n[k] != 0; k++) mvwprintw(win, 1, 2 + i + k + 1, "%c", n[k]);
        wrefresh(win);
        j++;
    }
    n[j] = 0;
    tmp = realloc(name, strlen(n));
    if (tmp != NULL)
        strcpy(name, n);
    
    curs_set(0);
    destroy_win(win);
}

void play_agility_game(int y_max, int x_max)
{
    WINDOW* win;
    unsigned int i = 1;
    unsigned int score = 0;

    win = create_newwin(6, x_max - 12, y_max - 8, 5);
    while (i <= 10)
    {
        if (agility(y_max, x_max, 6))
        {
            while (1)
            {
                box(win, 0 , 0);
                mvwprintw(win, 1, 2, "You have passed the %s agility test", int_to_word(i));
                if (wgetch(win) == 10) break;
            }
            score++;
        }
        else
        {
            while (1)
            {
                box(win, 0 , 0);
                mvwprintw(win, 1, 2, "You failed the %s agility test", int_to_word(i));
                if (wgetch(win) == 10) break;
            }
        }
        wclear(win);
        wrefresh(win);
        i++;
    }

    box(win, 0 , 0);
    mvwprintw(win, 1, 2, "Your score is : %d out of 10", score);
    while (1)
    {
        if (wgetch(win) == 10) break;
    }

    destroy_win(win);
}
