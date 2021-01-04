#include "data/libs/lib.h"

#define DEFAULT_TEXT_SPEED_0 (50000)
#define DEFAULT_TEXT_SPEED_1 (10000)
#define DEFAULT_AGILITY_SPEED (10000)
#define DEFAULT_MAX_NAME_SIZE (10)
#define DEFAULT_LANGUAGE ("english")

void play_menu(int, int, char *);
void play_game(int, int, char *, char *, char *);
void play_agility_game(int, int, int, char *);
char *ask_new_name(int, int, size_t, char *);

int main()
{
    /* BASICS VARs */
    char *language = NULL;
    char *buffer = NULL;
    int y_max, x_max;

    buffer = open_file("data/data.json", "r"); /* get json data file */

    if (buffer == NULL) /* if the file does not exist, exit*/
        return 0;

    language = get_json_object_string("default_language", buffer);

    if (language == NULL) /* if there is no default language, load default one */
        language = strdup(DEFAULT_LANGUAGE);

    /* START NCURSES */
    initscr();
    getmaxyx(stdscr, y_max, x_max);
    noecho();
    cbreak();
    curs_set(0);

    /*
    printw("%s\n", language);
    refresh();
    getchar();*/

    play_menu(y_max, x_max, language); /* play the menu */

    endwin(); /* end ncurses */

    free(buffer);
    buffer = NULL;

    return 0;
}

void play_menu(int y_max, int x_max, char *language)
{
    /* BASIC VARs */
    char *languages_answer = NULL;
    int menu_answer;
    int agility_speed;
    /* JSON VARs */
    char *usr_buffer = NULL;
    char *buffer = NULL;
    FILE *user = NULL;
    /* NAME VARs */
    char *name = NULL;
    int max_name_size;

    /* READ AND GET JSON DATA */
    user = fopen("data/user.json", "r");

    buffer = open_file("data/data.json", "r");

    /*
    printw("current language is: %s", language);
    refresh();
    getchar();*/

    /* DISPLAY THE GAME MENU */
    menu_answer = display_menu(y_max, x_max, language, buffer);

    /* TODO: choose language
    Polyglot : The game supports at least one language other than english */

    /* MENU PLAYER CHOICE */
    if (menu_answer == 0)
    {
        if (user == NULL)              /* means that the user play for the first time */
            create_player_json_data(); /* after this function call, the file user.json is created */

        usr_buffer = open_file("data/user.json", "r"); /* open user file */

        name = get_json_object_string("name", usr_buffer); /* get the player name key */
        if (!name || *name == 0)                           /* if it's empty, ask again a name */
        {
            max_name_size = get_json_data_int("max_name_size", buffer);
            if (!max_name_size)
                max_name_size = DEFAULT_MAX_NAME_SIZE;

            free(name);
            name = NULL;

            name = ask_new_name(y_max, x_max, max_name_size, buffer);

            set_json_object_string("name", name, usr_buffer);

            /* RELOAD USER BUFFER */
            free(usr_buffer);
            usr_buffer = NULL;

            usr_buffer = open_file("data/user.json", "r");
        }

        play_game(y_max, x_max, buffer, usr_buffer, name);
    }
    else if (menu_answer == 1)
    {
        agility_speed = get_json_data_int("agility_speed", buffer); /* default agility speed */
        if (!agility_speed)
            agility_speed = DEFAULT_AGILITY_SPEED; /* if agility_speed is NULL, load static default agility speed 1 */

        play_agility_game(y_max, x_max, agility_speed, language);
    }
    else if (menu_answer == 2)
    {
        languages_answer = display_languages(y_max, x_max, language, buffer);

        if (languages_answer != NULL)
        {
            /*
            printw("new language incoming is: %s", languages_answer);
            refresh();
            getchar();*/

            play_menu(y_max, x_max, languages_answer);
        }
    }
    else if (menu_answer == 3)
    {
        print_credits(y_max, x_max, language);
    }

    if (user)
    {
        fclose(user);
    }

    /* FREES */
    free(buffer);
    buffer = NULL;

    free(usr_buffer);
    usr_buffer = NULL;

    free(name);
    name = NULL;

    free(language);
    language = NULL;
}

void play_game(int y_max, int x_max, char *buffer, char *usr_buffer, char *name)
{
    /* STORY JSON VARs */
    struct json_object *parsed_json;
    struct array_list *parsed_story;
    chapter **story;
    /* STORY DATA VARs */
    unsigned int speed_0, speed_1;
    unsigned int agility_speed;
    unsigned int i;

    /* GET GAME SETTINGS DATA */
    speed_0 = get_json_data_int("text_speed_0", buffer); /* default text speed */
    if (!speed_0)
        speed_0 = DEFAULT_TEXT_SPEED_0;                  /* if speed_0 is NULL, load static default text speed 0 */
    speed_1 = get_json_data_int("text_speed_1", buffer); /* skipped text speed */
    if (!speed_1)
        speed_1 = DEFAULT_TEXT_SPEED_1;                         /* if speed_1 is NULL, load static default text speed 1 */
    agility_speed = get_json_data_int("agility_speed", buffer); /* default agility speed */
    if (!agility_speed)
        agility_speed = DEFAULT_AGILITY_SPEED; /* if agility_speed is NULL, load static default agility speed 1 */

    /* GET GAME DATA */
    parsed_json = json_tokener_parse(buffer);
    parsed_story = json_object_get_array(json_object_object_get(parsed_json, "story"));

    /* STORE DATA */
    story = get_story_data(parsed_story, buffer);

    for (i = 0; story[i]; i++)
    {
        display_title(y_max, x_max, parsed_story, i);

        begin_chapter(y_max, x_max, speed_0, speed_1, agility_speed, story[i], i, parsed_story, name, buffer, usr_buffer);
    }

    free_story_data(story);
    json_object_put(parsed_json); /* if res == 1 mean that it's a success */
}

char *ask_new_name(int y_max, int x_max, size_t max_size, char *buffer)
{
    WINDOW *win;
    char *name = NULL; /* final user name */
    char *ask = NULL;  /* the sentence that ask the user name */
    char *temp = NULL; /* temp char* that contain the player name */
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;
    int wait_time; /* text speed */
    int c;         /* user char entry */

    /* WINDOW SETTINGS */
    win = create_newwin(8, x_max - 4, y_max - 9, 2);
    curs_set(1); /* put the cursor on */

    /* ASK THE QUESTION */
    wait_time = get_json_data_int("text_speed_0", buffer); /* default text speed */
    if (!wait_time)
        wait_time = DEFAULT_TEXT_SPEED_0;
    ask = strdup("Enter your name:");

    for (i = 0; ask[i]; i++)
    {
        mvwprintw(win, 1, 2 + i, "%c", ask[i]);
        fflush(stdout);
        usleep(wait_time);
        wrefresh(win);
    }

    /* GET USER INPUT */
    temp = malloc(sizeof(char) * ((int)max_size + 1));

    while (j < (unsigned int)max_size)
    {
        c = wgetch(win);
        if (c == 10)
            break;
        else if (c == 127)
        {
            temp[j] = 0;
            j--;
            temp[j] = 0;
            j--;
        }
        else
            temp[j] = c;

        clear_win(win);
        mvwprintw(win, 1, 2, ask);
        for (k = 0; k <= j; k++)
            mvwprintw(win, 1, 2 + i + k + 1, "%c", temp[k]);

        wrefresh(win);
        j++;
    }

    temp[j] = 0;

    name = strdup(temp);

    curs_set(0); /* put the cursor off */

    free(ask);
    ask = NULL;

    free(temp);
    temp = NULL;

    if (strlen(name) < 3)
    {
        mvwprintw(win, 3, 2, "You need atleast 3 characters in your name !");
        wrefresh(win);
        getchar();

        free(name);
        name = NULL;

        name = ask_new_name(y_max, x_max, max_size, buffer);
    }

    destroy_win(win);

    return name;
}

void play_agility_game(int y_max, int x_max, int speed, char *language)
{
    /* BASICS VARs */
    WINDOW *win;
    char *language_cpy = strdup(language);
    int height = y_max / 4;
    int width = x_max - 4;
    unsigned int score = 0;
    unsigned int i = 1;

    /* WINDOW SETTINGS */
    win = newwin(height, width, y_max - height - 1, 2);
    wrefresh(win);

    while (i <= 10)
    {
        if (agility(y_max, x_max, 6, speed))
        {
            while (1)
            {
                box(win, 0, 0);
                if (strcmp(language, "english") == 0)
                    mvwprintw(win, 1, 2, "You have passed the %s agility test", int_to_word(i));
                else if (strcmp(language, "french") == 0)
                    mvwprintw(win, 1, 2, "Vous avez reussi le %s test d'agilite", int_to_word_fr(i));

                if (wgetch(win) == 10)
                    break;
            }
            score++;
        }
        /* means that it's a failure */
        else
        {
            while (1)
            {
                box(win, 0, 0);
                if (strcmp(language, "english") == 0)
                    mvwprintw(win, 1, 2, "You failed the %s agility test", int_to_word(i));
                else if (strcmp(language, "french") == 0)
                    mvwprintw(win, 1, 2, "Vous avez echoue au %s test d'agilite", int_to_word_fr(i));

                if (wgetch(win) == 10)
                    break;
            }
        }
        wclear(win);
        wrefresh(win);
        i++;
    }

    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Your score is : %d out of 10", score);
    while (1)
    {
        if (wgetch(win) == 10)
            break;
    }

    destroy_win(win);

    play_menu(y_max, x_max, language_cpy);
}
