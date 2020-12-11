#include <json-c/json.h>
#include "lib.h"

size_t get_json_options_length(char* key, char* buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *options; /* json object options */
	size_t n_options;

	parsed_json = json_tokener_parse(buffer);
    json_object_object_get_ex(parsed_json, key, &options);

	n_options = json_object_array_length(options);

    return n_options;
}

void get_json_array_data(char** data, char* key, char* buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *options; /* json object options */
	struct json_object *option; /* json object option index */
	size_t n_options;

    size_t i;

	parsed_json = json_tokener_parse(buffer);
    json_object_object_get_ex(parsed_json, key, &options);

	n_options = json_object_array_length(options);

    for(i = 0; i < n_options; i++)
    {
		option = json_object_array_get_idx(options, i);
        data[i] = (char*)json_object_get_string(option);
	}
}

char* get_json_data(char* key, char* buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *data; /* json object data */

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, key, &data);

	return (char*)json_object_get_string(data);
}

bool get_json_data_boolean(char* key, char* buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *data; /* json object data */

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, key, &data);

	return (bool)json_object_get_boolean(data);
}

int get_json_data_int(char* key, char* buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *data; /* json object data */

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, key, &data);

	return (int)json_object_get_int(data);
}

void get_json_object_data(char* object_key, char* buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *data; /* json object data */

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, object_key, &data);

	json_object_object_foreach(data, key, val)
	{
		printf("key: %s, ", key);
		printf("val: %s\n", (char*)json_object_get_string(val));
	}
}

int change_json_object_data(char* object_key, unsigned int index_key, char* value_to_assign, char* buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *data; /* json object data */
	unsigned int i = 0;
	int answer = 0;

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, object_key, &data);

	json_object_object_foreach(data, key, val)
	{
		if (i == index_key)
		{
			if (json_object_set_string(val, value_to_assign) == 1)
			{
				answer = 1;
				printf("\nchanged \"%s\" key with value \"%s\"\n\n", key, value_to_assign);
			}
		}
		i++;
	}

	return answer;
}
