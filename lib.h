#ifndef hi
#define hi

#include <ncurses/ncurses.h>

/* json-lib.c */
char**  get_json_array_data(char** data, char* key, char* buffer);
char*   get_json_data(char* key, char* buffer);
size_t  get_json_options_length(char* buffer, char* key);

/* window-lib.c */
WINDOW* create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void clear_win(WINDOW *local_win);

/* menu-lib.c */
void display_menu(int y_max, int x_max);

#endif