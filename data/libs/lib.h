#ifndef hi
#define hi

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include <json.h>

#ifdef __linux__
#include <ncurses.h>
#elif _WIN32
#include <ncurses/ncurses.h>
#endif

#define IS_LOWER_CASE(c) ((c >= 'a' && c <= 'z'))
#define GAME_DATA_PATH ("data/data.json")
#define USER_DATA_PATH ("data/user.json")
#define DEFAULT_TEXT_SPEED_0 (17500)
#define DEFAULT_TEXT_SPEED_1 (5000)
#define DEFAULT_AGILITY_SPEED (100000)
#define DEFAULT_MAX_NAME_SIZE (10)
#define DEFAULT_LANGUAGE ("English")

typedef struct
{
    char *key;
    char *text;
    json_object *upgrade;
    array_list *choices;
    json_object *test;
    json_object *date;
    char *next_key;
} part;

typedef struct
{
    char *title;
    part **parts;
    int length;
} chapter;

/* game.c */
void play_menu(int y_max, int x_max, char **language_ptr, char *);

/* json-lib.c */
void get_json_array_data(char **data, char *key, char *buffer);
size_t get_json_options_length(char *key, char *buffer);
bool get_json_data_boolean(char *key, char *buffer);
int get_json_data_int(char *key, char *buffer);
char *get_array_idx_key(char *buffer);
int set_json_object_int(char *key, const int new_value, char **buffer);
char *get_json_object_string(const char *key, const char *buffer);
int set_json_object_string(char *key, const char *new_value, char *buffer);
char *get_json_object_index_data(const char *object_key, unsigned int index_key, const char *buffer);
void create_player_json_data(void);

/* window-lib.c */
WINDOW *create_newwin(int height, int width, int starty, int startx);
WINDOW *create_window_var(int y_max, int x_max, int position);
void refresh_window_var(WINDOW *local_win, int value, char *title);
void destroy_win(WINDOW *local_win);
void clear_win(WINDOW *local_win);

/* menu-lib.c */
int display_menu(int y_max, int x_max, char **language_ptr, char *buffer);
char *display_languages(int y_max, int x_max, char *buffer);

/* agility-lib.c */
bool agility(int y_max, int x_max, int size, int speed);

/* story-lib.c */
void begin_chapter(int y_max, int x_max, int speed_0, int speed_1, int agility_speed, char **language_ptr, chapter *chap, unsigned int chapter_index, array_list *parsed_story, char *name, char *buffer, char **usr_buffer);
void write_text(char **story, WINDOW *win, int y_max, int x_max, int speed_0, int speed_1, char *name);
char *get_user_choices(WINDOW *win, array_list *choices, char *usr_buffer);
char *get_part_date(part *dpart);

/* game-lib.c */
char **sentence_separator(char *str, char *separator);
char *int_to_word(int n);
char *int_to_word_fr(int n);
char *open_file(char *path, char *access_mode);
void refresh_windows_vars(int agilityval, int mentalval, int trustval, WINDOW *agilitywin, WINDOW *mentalwin, WINDOW *trustwin);
void destroy_windows_vars(WINDOW *agilitywin, WINDOW *mentalwin, WINDOW *trustwin);
int add_agility_value(const int add_value, char **buffer);
int add_mental_value(const int add_value, char **buffer);
int add_trust_value(const int add_value, char **buffer);
int add_first_choice_value(const int add_value, char **buffer);
int add_second_choice_value(const int add_value, char **buffer);
int add_third_choice_value(const int add_value, char **buffer);
int increment_save_chapter_index(char **buffer);
void reset_skill_values(char **buffer);
int reset_save_chapter_index(char **buffer);
void display_title(int y_max, int x_max, array_list *story, unsigned int chapter_index);
void print_credits(int y_max, int x_max);
int reload_credits_win(WINDOW *win, int y_pos, int x_pos);
/* data story oriented lib */
json_object *get_part_text_data_by_key(array_list *story, char *key, int chapter_index, const char *buffer);
chapter **get_story_data(array_list *story, char *buffer);
chapter *get_chapter_data(array_list *story, int chapter_index, char *buffer);
part *get_part_data(array_list *story, char *key, int chapter_index, const char *buffer);
char *get_first_key(unsigned int chapter_index, char *buffer);
void free_part(part *dpart);
void free_chapter_data(chapter *data);
void free_story_data(chapter **story);

#endif
