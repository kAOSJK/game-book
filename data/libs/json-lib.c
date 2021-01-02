#include <string.h>
#include "lib.h"

#define GAME_DATA_PATH ("data/data.json")
#define USER_DATA_PATH ("data/user.json")

size_t get_json_options_length(char *key, char *buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *options;	 /* json object options */
	size_t n_options;

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, key, &options);

	n_options = json_object_array_length(options);

	json_object_put(parsed_json);

	return n_options;
}

void get_json_array_data(char **data, char *key, char *buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *options;	 /* json object options */
	struct json_object *option;		 /* json object option index */
	size_t n_options;
	size_t i;

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, key, &options);

	n_options = json_object_array_length(options);

	for (i = 0; i < n_options; i++)
	{
		option = json_object_array_get_idx(options, i);
		data[i] = strdup((char *)json_object_get_string(option));
		if (data[i] == NULL)
			printf("error: bad strdup");
	}

	json_object_put(parsed_json);
}

bool get_json_data_boolean(char *key, char *buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *data;		 /* json object data */
	bool res;

	if (key == NULL)
	{
		fprintf(stderr, "error: key argument is null\n");
		return NULL;
	}

	if (buffer == NULL)
	{
		fprintf(stderr, "error: buffer argument is null\n");
		return NULL;
	}

	parsed_json = json_tokener_parse(buffer);

	if (parsed_json == NULL)
	{
		fprintf(stderr, "error: parsed_json is null\n");
		return NULL;
	}

	json_object_object_get_ex(parsed_json, key, &data);

	if (data == NULL)
	{
		fprintf(stderr, "error: data is null\n");
		json_object_put(parsed_json);
		return NULL;
	}

	res = (bool)json_object_get_boolean(data);

	json_object_put(parsed_json);

	return res;
}

int get_json_data_int(char *key, char *buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *data;		 /* json object data */
	int n;

	if (key == NULL)
	{
		fprintf(stderr, "error: key argument is null\n");
		return 0;
	}

	if (buffer == NULL)
	{
		fprintf(stderr, "error: buffer argument is null\n");
		return 0;
	}

	parsed_json = json_tokener_parse(buffer);

	if (parsed_json == NULL)
	{
		fprintf(stderr, "error: parsed_json is null\n");
		return 0;
	}

	json_object_object_get_ex(parsed_json, key, &data);

	if (data == NULL)
	{
		fprintf(stderr, "error: data is null\n");
		json_object_put(parsed_json);
		return 0;
	}

	n = (int)json_object_get_int(data);

	json_object_put(parsed_json);

	return n;
}

/* OBJECT ORIENTED JSON-LIB */

char *get_array_idx_key(char *buffer)
{
	char *answer = NULL;
	char *token;

	token = strtok(buffer, "\"");
	token = strtok(NULL, "\"");

	answer = malloc(strlen(token) + 1);
	strcpy(answer, token);

	return answer;
}

int get_json_object_int(const char *key, const char *buffer)
{
	struct json_object *parsed_json = json_tokener_parse(buffer);
	return (int)json_object_get_int(json_object_object_get(parsed_json, key));
}

int set_json_object_int(char *key, const int new_value, FILE *fp, char *buffer)
{
	struct json_object *parsed_json = json_tokener_parse(buffer);
	char *temp = NULL;
	int res;

	if (!key || *key == 0)
	{
		fprintf(stderr, "error: key is null\n");
		return 0;
	}

	if (!buffer || *buffer == 0)
	{
		fprintf(stderr, "error: buffer is null\n");
		return 0;
	}

	res = json_object_set_int(json_object_object_get(parsed_json, key), new_value);

	if (res == 1)
		json_object_to_file(USER_DATA_PATH, parsed_json); /* save json file */

	temp = open_file(fp, USER_DATA_PATH, "r");

	if (temp == NULL)
	{
		fprintf(stderr, "error: temp buffer is null\n");
		return 0;
	}

	strcpy(buffer, temp); /* reload buffer */

	free(temp);
	temp = NULL;

	json_object_put(parsed_json);

	return 1;
}

char *get_json_object_string(const char *key, const char *buffer)
{
	char *str = NULL;
	struct json_object *parsed_json = NULL;
	struct json_object *obj = NULL;

	if (key == NULL)
	{
		fprintf(stderr, "error: key is null\n");
		return NULL;
	}

	if (buffer == NULL)
	{
		fprintf(stderr, "error: buffer is null\n");
		return NULL;
	}

	parsed_json = json_tokener_parse(buffer);

	if (parsed_json == NULL)
	{
		fprintf(stderr, "error: parsed_json is null\n");
		return NULL;
	}

	int ret = json_object_object_get_ex(parsed_json, key, &obj);

	if (obj == NULL || ret == 0)
	{
		fprintf(stderr, "error: obj is null OR ret is null\n");
		return NULL;
	}

	str = strdup((char *)json_object_get_string(obj));

	if (str == NULL)
	{
		fprintf(stderr, "error: str after strdup is null\n");
		return NULL;
	}

	json_object_put(parsed_json);

	return str;
}

int set_json_object_string(char *key, const char *new_value, FILE *fp, char *buffer)
{
	struct json_object *parsed_json = json_tokener_parse(buffer);
	char *temp = NULL;
	int res;

	if (!key || *key == 0)
	{
		fprintf(stderr, "error: key is null\n");
		return 0;
	}

	if (!buffer || *buffer == 0)
	{
		fprintf(stderr, "error: buffer is null\n");
		return 0;
	}

	res = json_object_set_string(json_object_object_get(parsed_json, key), new_value);

	if (res == 1)
		json_object_to_file(USER_DATA_PATH, parsed_json); /* save json file */

	temp = open_file(fp, USER_DATA_PATH, "r");

	if (temp == NULL)
	{
		fprintf(stderr, "error: temp buffer is null\n");
		return 0;
	}

	/*
	free(buffer);
	buffer = NULL;

	buffer = strdup(temp); reload buffer
	*/

	printw("\nfunction: %s\n", buffer);
	refresh();
	getchar();

	free(temp);
	temp = NULL;

	json_object_put(parsed_json);

	return 1;
}

void create_player_json_data()
{
	struct json_object *nobj;

	nobj = json_object_new_object(); /* create new object on nobj */

	/* ADD KEYS AND DATA */
	json_object_object_add(nobj, "name", json_object_new_string(""));
	json_object_object_add(nobj, "agility", json_object_new_int(0));
	json_object_object_add(nobj, "mental", json_object_new_int(0));
	json_object_object_add(nobj, "trust", json_object_new_int(0));

	json_object_to_file(USER_DATA_PATH, nobj); /* save the file */

	json_object_put(nobj);
}
