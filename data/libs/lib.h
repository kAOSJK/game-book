#ifndef hi
#define hi

#include <json.h>

#ifdef __linux__
#include <ncurses.h>
#elif _WIN32
#include <ncurses/ncurses.h>
#endif

#define IS_LOWER_CASE(c) ((c >= 'a' && c <= 'z'))

typedef struct
{
    char *key;
    char *text;
    json_object *upgrade;
    array_list *choices;
    json_object *test;
    char *next_key;
} part;

typedef struct
{
    char *title;
    part **parts;
    int length;
} chapter;

/* game.c */
void play_menu(int y_max, int x_max, char *language);

/* json-lib.c */
void get_json_array_data(char **data, char *key, char *buffer);
size_t get_json_options_length(char *key, char *buffer);
bool get_json_data_boolean(char *key, char *buffer);
int get_json_data_int(char *key, char *buffer);
/* object oriented lib */
char *get_array_idx_key(char *buffer);
int get_json_object_int(const char *key, const char *buffer);
int set_json_object_int(char *key, const int new_value, FILE *fp, char *buffer);
char *get_json_object_string(const char *key, const char *buffer);
void set_json_object_string(char *key, const char *new_value, FILE *fp, char *buffer);
char *get_json_object_index_data(const char *object_key, unsigned int index_key, const char *buffer);
void create_player_json_data(void);

/* window-lib.c */
WINDOW *create_newwin(int height, int width, int starty, int startx);
WINDOW *create_windows_vars(int y_max, int x_max, int position, const char *title, int value);
void destroy_win(WINDOW *local_win);
void clear_win(WINDOW *local_win);

/* menu-lib.c */
int display_menu(int y_max, int x_max, char *language, char *buffer);
char *display_languages(int y_max, int x_max, char *language, char *buffer);

/* agility-lib.c */
bool agility(int y_max, int x_max, int size);

/* story-lib.c */
void begin_chapter(int y_max, int x_max, int speed_0, int speed_1, chapter *chap, unsigned int chapter_index, array_list *parsed_story, char *name, char *buffer, char *usr_buffer);
void write_text(char **story, WINDOW *win, int y_max, int x_max, int speed_0, int speed_1, char *name);
char *get_user_choices(WINDOW *win, array_list *choices, char *usr_buffer);

/* game-lib.c */
char **sentence_separator(char *str, char *separator);
char *int_to_word(int n);
void update_json(char *temp, FILE *out);
char *open_file(FILE *fp, char *path, char *access_mode);
void reload_windows_vars(int y_max, int x_max, int agilityval, int mentalval, int trustval, WINDOW *agilitywin, WINDOW *mentalwin, WINDOW *trustwin);
int add_agility_value(const int add_value, FILE *fp, char *buffer);
int add_mental_value(const int add_value, FILE *fp, char *buffer);
int add_trust_value(const int add_value, FILE *fp, char *buffer);
/* data story oriented lib */
json_object *get_part_text_data_by_key(array_list *story, char *key, int chapter_index, const char *buffer);
chapter **get_story_data(array_list *story, char *buffer);
chapter *get_chapter_data(array_list *story, int chapter_index, char *buffer);
part *get_part_data(array_list *story, char *key, int chapter_index, const char *buffer);
char *get_first_key(unsigned int chapter_index, char *buffer);
void free_part(part *dpart);
void free_chapter_data(chapter *data);
void free_story_data(chapter **story);
void print_credits(int y_max, int x_max, char *language);
int reload_credits_win(WINDOW *win, int y_pos, int x_pos);

#endif
