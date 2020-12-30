#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "lib.h"

#define CREDITS_WAIT_TIME (2500000)

char **sentence_separator(char *str, char *separator)
{
    char **array = malloc(sizeof(char *));
    char **tmp = NULL;
    char *token = NULL;
    size_t n;

    if (array)
    {
        n = 1;
        token = strtok(str, separator);
        while (token)
        {
            tmp = realloc(array, (n + 1) * sizeof(char *));
            if (tmp == NULL)
                break;

            array = tmp;
            ++n;

            array[n - 2] = malloc(strlen(token) + 1);
            if (array[n - 2] != NULL)
                strcpy(array[n - 2], token);
            token = strtok(NULL, separator);
        }

        array[n - 1] = NULL;
    }

    return array;
}

char *int_to_word(int n)
{
    if (n == 1)
        return "first";
    else if (n == 2)
        return "second";
    else if (n == 3)
        return "third";
    else if (n == 4)
        return "fourth";
    else if (n == 5)
        return "fifth";
    else if (n == 6)
        return "sixth";
    else if (n == 7)
        return "seventh";
    else if (n == 8)
        return "eigth";
    else if (n == 9)
        return "ninth";
    else if (n == 10)
        return "tenth";
    else
        return "\0";
}

void update_json(char *buffer, FILE *out)
{
    out = fopen("data/player.json", "w");
    fputs(buffer, out);
    free(buffer);
    buffer = NULL;
    fclose(out);
}

char *open_file(FILE *fp, char *path, char *access_mode)
{
    char *buffer = NULL;
    size_t size;

    fp = fopen(path, access_mode);
    if (fp != NULL)
    {
        /* GET FILE SIZE */
        fseek(fp, 0L, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        /* MALLOC AND PUT FILE CONTENT */
        buffer = malloc((sizeof(unsigned char) * size) + 1);
        if (!buffer)
            fprintf(stderr, "error: allocation of the pointer failed\n");
        else
        {
            memset(buffer, 0x00, (sizeof(unsigned char) * size) + 1);
            fread(buffer, size, 1, fp);
            buffer[size] = '\0';
        }
    }

    fclose(fp);

    return buffer;
}

void reload_windows_vars(int y_max, int x_max, int agilityval, int mentalval, int trustval, WINDOW *agilitywin, WINDOW *mentalwin, WINDOW *trustwin)
{
    agilitywin = create_windows_vars(y_max, x_max, 0, "AGILITY", agilityval);
    mentalwin = create_windows_vars(y_max, x_max, 1, "MENTAL", mentalval);
    trustwin = create_windows_vars(y_max, x_max, 2, "TRUST", trustval);
}

int add_agility_value(const int add_value, FILE *fp, char *buffer)
{
    int new_agility = get_json_data_int("agility", buffer) + add_value;
    int res;

    set_json_object_int("agility", new_agility, fp, buffer);

    return res;
}

int add_mental_value(const int add_value, FILE *fp, char *buffer)
{
    int new_mental = get_json_data_int("mental", buffer) + add_value;
    int res;

    res = set_json_object_int("mental", new_mental, fp, buffer);

    return res;
}

int add_trust_value(const int add_value, FILE *fp, char *buffer)
{
    int new_trust = get_json_data_int("trust", buffer) + add_value;
    int res;

    res = set_json_object_int("trust", new_trust, fp, buffer);

    return res;
}

/* data story oriented lib */

json_object *get_part_text_data_by_key(array_list *story, char *key, int chapter_index, const char *buffer)
{
    /* GET CHAPTER NAME */
    char *chapter_name = get_array_idx_key((char *)json_object_to_json_string(array_list_get_idx(story, chapter_index)));
    /* THE ENTIRE RESEARCHED CHAPTER */
    struct array_list *chapter = json_object_get_array(json_object_object_get(array_list_get_idx(story, chapter_index), chapter_name));
    size_t size = array_list_length(chapter);

    char *temp = NULL;
    json_object *obj = NULL;
    unsigned int i;

    for (i = 0; i < size && obj == NULL; i++)
    {
        temp = get_json_object_string("key", json_object_to_json_string(array_list_get_idx(chapter, i)));
        if (temp != NULL)
        {
            if (strcmp(temp, key) == 0)
                obj = array_list_get_idx(chapter, i);
        }

        free(temp);
        temp = NULL;
    }

    free(chapter_name);

    return obj;
}

chapter **get_story_data(array_list *story, char *buffer)
{
    /* STORY SIZE */
    size_t story_size = array_list_length(story);

    chapter **answer = malloc(sizeof(chapter *) * (story_size + 1));
    unsigned int i = 0;

    if (answer == NULL)
    {
        fprintf(stderr, "error: allocation of the pointer failed\n");
        return NULL;
    }

    for (i = 0; i < (unsigned int)story_size; i++)
        answer[i] = get_chapter_data(story, i, buffer);

    answer[i] = NULL;

    return answer;
}

chapter *get_chapter_data(array_list *story, int chapter_index, char *buffer)
{
    char *chapter_name = get_array_idx_key((char *)json_object_to_json_string(array_list_get_idx(story, chapter_index)));
    /* THE SIZE OF THE ENTIRE RESEARCHED CHAPTER */
    size_t size = array_list_length(json_object_get_array(json_object_object_get(array_list_get_idx(story, chapter_index), chapter_name)));
    /* STRUCT CHAPTER THAT WILL CONTAIN A WHOLE CHAPTER */
    chapter *nchapter = (chapter *)malloc(sizeof(chapter));
    /* OTHER VARs */
    char *next_key = NULL;
    unsigned int i;

    /* MALLOC AND PUT CHAPTER'S TITLE */
    nchapter->title = strdup(chapter_name);

    /* GET ALL THE DATA */
    nchapter->parts = (part **)malloc(sizeof(part *) * size);
    if (nchapter->parts != NULL)
    {
        for (i = 0; i < size; i++)
        {
            next_key = get_json_object_string("key", json_object_to_json_string(array_list_get_idx(json_object_get_array(json_object_object_get(array_list_get_idx(story, chapter_index), chapter_name)), i)));

            if (next_key != NULL && *next_key != 0)
            {
                nchapter->parts[i] = get_part_data(story, next_key, chapter_index, buffer);
                free(next_key);
                next_key = NULL;
            }
            else
                fprintf(stderr, "error: some keys are wrong on the data.JSON file, please check it!\n");
        }

        nchapter->length = size;
    }
    else
        fprintf(stderr, "error: allocation of the pointer failed\n");

    /*
    free(next_key);*/
    free(chapter_name);

    return nchapter;
}

part *get_part_data(array_list *story, char *key, int chapter_index, const char *buffer)
{
    part *npart = (part *)malloc(sizeof(part));
    json_object *obj;
    json_object *obj_key;
    json_object *obj_text;
    json_object *obj_upgrade;
    array_list *obj_choices;
    json_object *obj_test;
    json_object *obj_next_key;

    npart->key = NULL;
    npart->text = NULL;
    npart->upgrade = NULL;
    npart->choices = NULL;
    npart->test = NULL;
    npart->next_key = NULL;

    if (npart == NULL)
    {
        fprintf(stderr, "error: allocation of the pointer failed\n");
        return NULL;
    }

    obj = get_part_text_data_by_key(story, key, chapter_index, buffer);
    if (obj == NULL)
    {
        free(npart);
        fprintf(stderr, "error: can't find key\n");
        return NULL;
    }

    json_object_object_get_ex(obj, "key", &obj_key);
    json_object_object_get_ex(obj, "text", &obj_text);
    json_object_object_get_ex(obj, "upgrade", &obj_upgrade);
    obj_choices = json_object_get_array(json_object_object_get(obj, "choices"));
    json_object_object_get_ex(obj, "test", &obj_test);
    json_object_object_get_ex(obj, "next_key", &obj_next_key);

    /* ALLOCATE AND PUT THE KEY TO THE NEW PART */
    if (obj_key != NULL)
    {
        npart->key = strdup(json_object_get_string(obj_key));
        if (npart->key == NULL)
            fprintf(stderr, "error: allocation of the pointer failed\n");
    }

    /* ALLOCATE AND PUT THE TEXT TO THE NEW PART */
    if (obj_text != NULL)
    {
        npart->text = strdup(json_object_get_string(obj_text));
        if (npart->text == NULL)
            fprintf(stderr, "error: allocation of the pointer failed\n");
    }

    /* PUT THE POTENTIAL UPGRADE TO THE NEW PART */
    if (obj_upgrade != NULL)
    {
        npart->upgrade = obj_upgrade;
    }

    /* ALLOCATE AND PUT CHOICES TO THE NEW PART IF THERE IS ONE */
    if (obj_choices != NULL)
    {
        npart->choices = obj_choices;
    }
    /* ALLOCATE AND PUT TEST TO THE NEW PART IF THERE IS ONE */
    else if (obj_test != NULL)
    {
        npart->test = obj_test;
    }
    /* ELSE IT MEANS THAT THERE IS JUST A LINEAR NEXT KEY */
    else if (obj_next_key != NULL)
    {
        npart->next_key = strdup(json_object_get_string(obj_next_key));
        if (npart->next_key == NULL)
        {
            fprintf(stderr, "error: allocation of the pointer failed\n");
            return NULL;
        }
    }

    return npart;
}

char *get_first_key(unsigned int chapter_index, char *buffer)
{
    char **entrys = NULL;
    char *key = NULL;
    size_t length;
    unsigned int i;

    length = get_json_options_length("entry_keys", buffer);
    entrys = malloc(sizeof(char *) * length);

    get_json_array_data(entrys, "entry_keys", buffer);

    key = strdup(entrys[chapter_index]);

    for (i = 0; i < (unsigned int)length; i++)
        free(entrys[i]);
    free(entrys);

    return key;
}

void free_part(part *dpart)
{
    free(dpart->key);
    dpart->key = NULL;

    free(dpart->text);
    dpart->text = NULL;

    free(dpart->next_key);
    dpart->next_key = NULL;

    free(dpart);
    dpart = NULL;
}

void free_chapter_data(chapter *data)
{
    unsigned int size = data->length;
    unsigned int i;

    for (i = 0; i < size; i++)
        free_part(data->parts[i]);

    free(data->title);
    data->title = NULL;

    free(data->parts);
    data->parts = NULL;

    free(data);
    data = NULL;
}

void free_story_data(chapter **story)
{
    unsigned int i;

    for (i = 0; story && story[i]; i++)
    {
        free_chapter_data(story[i]);
        story[i] = NULL;
    }

    story[i] = NULL;
    free(story);
    story = NULL;
}

void print_credits(int y_max, int x_max)
{
    WINDOW *win;
    FILE *file;
    int height = 11;
    int weight = 11;
    int c; /* represent the current readed char in the fie */

    create_newwin(height, weight, y_max / 2 - (height / 2), x_max / 2 - (weight / 2));

    file = fopen("data/lion_0", "r");
    if (file != NULL)
    {
        while ((c = getc(file)) != EOF)
            printw("%c", c);
        fclose(file);
        refresh();
    }

    refresh();
    fflush(stdout);
    usleep(CREDITS_WAIT_TIME);
    clear();

    file = fopen("data/lion_1", "r");
    if (file != NULL)
    {
        while ((c = getc(file)) != EOF)
            printw("%c", c);
        fclose(file);
        refresh();
    }

    refresh();
    fflush(stdout);
    usleep(CREDITS_WAIT_TIME);
    clear();

    file = fopen("data/lion_2", "r");
    if (file != NULL)
    {
        while ((c = getc(file)) != EOF)
            printw("%c", c);
        fclose(file);
        refresh();
    }
}
