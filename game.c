#include "data/libs/lib.h"

#define DEFAULT_TEXT_SPEED_0 (50000)
#define DEFAULT_TEXT_SPEED_1 (10000)
#define DEFAULT_AGILITY_SPEED (10000)
#define DEFAULT_MAX_NAME_SIZE (10)
#define DEFAULT_LANGUAGE ("English")

void play_game(int, int, char *, char **, char *, char *);
void play_agility_game(int, int, int, char *);
char *ask_new_name(int, int, size_t, char *, char *);

int main()
{
    /* BASICS VARs */
    char *language = NULL;
    char *buffer = NULL;
    int y_max, x_max;

    buffer = open_file(GAME_DATA_PATH, "r"); /* get json data file */

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

    /* PLAY THE MENU */
    play_menu(y_max, x_max, &language);

    free(buffer);
    buffer = NULL;

    free(language);
    language = NULL;

    endwin(); /* end ncurses */

    return 0;
}

void play_menu(int y_max, int x_max, char **language_ptr)
{
    /* BASIC VARs */
    int menu_answer;
    int agility_speed;
    char *language = *language_ptr;
    /* JSON VARs */
    char *usr_buffer = NULL;
    char *buffer = NULL;
    /* NAME VARs */
    char *name = NULL;
    int max_name_size;

    /* READ AND GET JSON DATA */
    usr_buffer = open_file(USER_DATA_PATH, "r");
    buffer = open_file(GAME_DATA_PATH, "r");

    /* DISPLAY THE GAME MENU */
    menu_answer = display_menu(y_max, x_max, language, buffer);

    /* MENU PLAYER CHOICE */
    if (menu_answer == 0)
    {
        if (usr_buffer == NULL) /* means that the user play for the first time */
        {
            create_player_json_data();                   /* after this function call, the file user.json is created */
            usr_buffer = open_file(USER_DATA_PATH, "r"); /* open user file */
        }

        name = get_json_object_string("name", usr_buffer); /* get the player name key */
        if (!name || *name == 0)                           /* if it's empty, ask again a name */
        {
            max_name_size = get_json_data_int("max_name_size", buffer);
            if (!max_name_size)
                max_name_size = DEFAULT_MAX_NAME_SIZE;

            name = ask_new_name(y_max, x_max, max_name_size, language, buffer);

            set_json_object_string("name", name, usr_buffer);

            /* RELOAD USER BUFFER */
            free(usr_buffer);
            usr_buffer = NULL;
            usr_buffer = open_file(USER_DATA_PATH, "r");
        }

        play_game(y_max, x_max, buffer, &usr_buffer, name, language);
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
        free(*language_ptr);
        *language_ptr = NULL;

        *language_ptr = display_languages(y_max, x_max, buffer);

        language = *language_ptr;

        /* TODO: corriger valgrind error here look for val.txt */
        if (language == NULL)
        {
            *language_ptr = strdup(DEFAULT_LANGUAGE);
            language = *language_ptr;

            fprintf(stderr, "error: unknown choosen language\n");
        }

        play_menu(y_max, x_max, language_ptr);
    }
    else if (menu_answer == 3)
    {
        print_credits(y_max, x_max, language);
    }

    /* FREES */
    free(buffer);
    buffer = NULL;

    free(usr_buffer);
    usr_buffer = NULL;

    free(name);
    name = NULL;
}

void play_game(int y_max, int x_max, char *buffer, char **usr_buffer_ptr, char *name, char *language)
{
    /* STORY JSON VARs */
    struct json_object *parsed_json = NULL;
    struct array_list *parsed_story = NULL;
    chapter **story;
    /* STORY DATA VARs */
    unsigned int saved_chapter_index = 0;
    unsigned int story_length = 0;
    unsigned int speed_0, speed_1;
    unsigned int agility_speed;
    unsigned int i;
    /* STORY LANGUAGES DATA VARs */
    char **languages_code = NULL;
    char **languages = NULL;
    char *story_language = NULL;
    char *code = NULL;
    unsigned int languages_length = 0;
    unsigned int languages_code_length = 0;
    unsigned int index = 0;
    /* USER DATA VARs */
    char *usr_buffer = *usr_buffer_ptr;

    /* GET GAME SETTINGS DATA */
    speed_0 = get_json_data_int("text_speed_0", buffer); /* default text speed */
    if (!speed_0)
        speed_0 = DEFAULT_TEXT_SPEED_0;                  /* if speed_0 is NULL, load static default text speed 0 */
    speed_1 = get_json_data_int("text_speed_1", buffer); /* skipped text speed */

    if (!speed_1)                                               /* default skipped text speed */
        speed_1 = DEFAULT_TEXT_SPEED_1;                         /* if speed_1 is NULL, load static default text speed 1 */
    agility_speed = get_json_data_int("agility_speed", buffer); /* default agility speed */

    if (!agility_speed)
        agility_speed = DEFAULT_AGILITY_SPEED; /* if agility_speed is NULL, load static default agility speed 1 */

    /* GET LANGUAGE MIN LENGTH, MALLOC IT, AND GET IT */
    languages_code_length = get_json_options_length("languages_code", buffer);
    languages_code = malloc(sizeof(char *) * languages_code_length);
    get_json_array_data(languages_code, "languages_code", buffer);

    /* GET LANGUAGE LENGTH, MALLOC IT, AND GET IT */
    languages_length = get_json_options_length("languages", buffer);
    languages = malloc(sizeof(char *) * languages_length);
    get_json_array_data(languages, "languages", buffer);

    /* GET THE RIGHT INDEX */
    for (i = 0; i < languages_length; i++)
    {
        if (strcmp(languages[i], language) == 0)
        {
            index = i;
            break;
        }
    }

    /* GET GAME DATA */
    parsed_json = json_tokener_parse(buffer);

    /* GET RIGHT STORY LANGUAGE */
    code = strdup(languages_code[index]);

    story_language = strdup("story");
    if (strcmp(code, "") != 0)
    {
        story_language = realloc(story_language, sizeof(char) * strlen(story_language) + strlen(code) + 1 + 1);
        strcat(story_language, "_");
        strcat(story_language, code);
    }

    /* GET GAME STORY DATA */
    parsed_story = json_object_get_array(json_object_object_get(parsed_json, story_language));

    if (parsed_story == NULL)
    {
        /* MESSAGE OF BAD CHOOSED LANGUAGE */
        clear();
        printw("\nTranslation of the choosen language is not finished, switching to the default language\n\nYou can change at any moment the default language in the data.json file");
        refresh();
        getchar();

        /* FREE LANGUAGE */
        free(story_language);
        story_language = NULL;

        /* LOAD DEFAULT LANGUAGE */
        story_language = strdup("story");
        if (strcmp(languages_code[0], "") != 0)
        {
            story_language = realloc(story_language, sizeof(char) * strlen(story_language) + strlen(languages_code[0]) + 1 + 1);
            strcat(story_language, "_");
            strcat(story_language, languages_code[0]);
        }

        parsed_story = json_object_get_array(json_object_object_get(parsed_json, story_language));

        if (parsed_story == NULL)
        {
            clear();
            printw("\nTranslation of the choosen default language is not finished, switching to static default language");
            refresh();
            getchar();

            parsed_story = json_object_get_array(json_object_object_get(parsed_json, "story"));
        }

        clear();
        refresh();
    }

    /* STORE DATA */
    story = get_story_data(parsed_story, buffer);

    /* GET STORY CHAPTERS LENGTH */
    for (i = 0; story[i]; i++)
        story_length++;

    for (i = 0; story[i]; i++)
    {
        saved_chapter_index = get_json_data_int("chapter_index", usr_buffer);
        while (saved_chapter_index > i)
            i++;

        if (i == story_length)
            break;

        /* DISPLAY THE TITLE */
        display_title(y_max, x_max, parsed_story, i);

        /* BEGIN THE CHAPTER */
        begin_chapter(y_max, x_max, speed_0, speed_1, agility_speed, language, story[i], i, parsed_story, name, buffer, usr_buffer_ptr);
        usr_buffer = *usr_buffer_ptr;
    }

    /* FREE LANGUAGE STORY VARs */
    for (i = 0; i < languages_code_length; i++)
    {
        free(languages_code[i]);
        languages_code[i] = NULL;
    }

    free(languages_code);
    languages_code = NULL;

    for (i = 0; i < languages_length; i++)
    {
        free(languages[i]);
        languages[i] = NULL;
    }

    free(languages);
    languages = NULL;

    free(code);
    code = NULL;

    free(story_language);
    story_language = NULL;

    /* FREE STORY VARs */
    free_story_data(story);
    json_object_put(parsed_json);

    /* RESET SAVED CHAPTER INDEX AND SKILLS */
    reset_save_chapter_index(usr_buffer_ptr);
    usr_buffer = *usr_buffer_ptr;

    reset_skill_values(usr_buffer_ptr);
    usr_buffer = *usr_buffer_ptr;

    /* PRINT CREDITS OF THE GAME AND GO BACK TO THE MENU */
    print_credits(y_max, x_max, language);
}

char *ask_new_name(int y_max, int x_max, size_t max_size, char *language, char *buffer)
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

    if (strcmp(language, "English") == 0)
        ask = strdup("Enter your name:");
    else if (strcmp(language, "Francais") == 0)
        ask = strdup("Entrer votre nom:");
    else if (strcmp(language, "Wong jawa") == 0)
        ask = strdup("Ketik jeneng sampeyan:");
    else
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

        name = ask_new_name(y_max, x_max, max_size, language, buffer);
    }

    destroy_win(win);

    return name;
}

void play_agility_game(int y_max, int x_max, int speed, char *language)
{
    /* BASICS VARs */
    WINDOW *win;
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
                if (strcmp(language, "English") == 0)
                    mvwprintw(win, 1, 2, "You have passed the %s agility test", int_to_word(i));
                else if (strcmp(language, "Francais") == 0)
                    mvwprintw(win, 1, 2, "Vous avez reussi le %s test d'agilite", int_to_word_fr(i));
                else if (strcmp(language, "Wong jawa") == 0)
                    mvwprintw(win, 1, 2, "Sampeyan wis lolos tes katrampilan iki");

                if (wgetch(win) == 10)
                    break;
            }
            score++;
        }
        /* MEAN THAT IT'S A FAILURE */
        else
        {
            while (1)
            {
                box(win, 0, 0);
                if (strcmp(language, "English") == 0)
                    mvwprintw(win, 1, 2, "You failed the %s agility test", int_to_word(i));
                else if (strcmp(language, "Francais") == 0)
                    mvwprintw(win, 1, 2, "Vous avez echoue au %s test d'agilite", int_to_word_fr(i));
                else if (strcmp(language, "Wong jawa") == 0)
                    mvwprintw(win, 1, 2, "Sampeyan gagal tes katrampilan iki");

                if (wgetch(win) == 10)
                    break;
            }
        }
        wclear(win);
        wrefresh(win);
        i++;
    }

    box(win, 0, 0);

    if (score >= 5)
    {
        if (strcmp(language, "English") == 0)
            mvwprintw(win, 1, 2, "Good job ! your score is: %d out of 10", score);
        else if (strcmp(language, "Francais") == 0)
            mvwprintw(win, 1, 2, "Bien joue ! votre score est: %d sur 10", score);
        else if (strcmp(language, "Wong jawa") == 0)
            mvwprintw(win, 1, 2, "Game apik! skor sampeyan: %d saka 10", score);
    }
    else
    {
        if (strcmp(language, "English") == 0)
            mvwprintw(win, 1, 2, "Your score is: %d out of 10", score);
        else if (strcmp(language, "Francais") == 0)
            mvwprintw(win, 1, 2, "Votre score est: %d sur 10", score);
        else if (strcmp(language, "Wong jawa") == 0)
            mvwprintw(win, 1, 2, "Skor sampeyan: %d saka 10", score);
    }

    while (1)
    {
        if (wgetch(win) == 10)
            break;
    }

    destroy_win(win);

    play_menu(y_max, x_max, &language);
}
