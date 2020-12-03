#ifndef hi
#define hi

#include <ncurses/ncurses.h>

/* json-lib.c */
void    get_json_array_data(char** data, char* key, char* buffer);
char*   get_json_data(char* key, char* buffer);
size_t  get_json_options_length(char* key, char* buffer);
bool    get_json_data_boolean(char* key, char* buffer);
int     get_json_data_int(char* key, char* buffer);

/* window-lib.c */
WINDOW* create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void clear_win(WINDOW *local_win);

/* menu-lib.c */
int display_menu(int y_max, int x_max, char* buffer);

/* agility-lib.c */
bool agility(int y_max, int x_max, int size);

/* story-lib.c */
void begin_story(int y_max, int x_max, int speed_0, int speed_1, int story_length, char** parts, bool show_debug, char* buffer);
void write_story(char** story, WINDOW* win, int speed_0, int speed_1);
char** sentence_separator(char* str);

#endif