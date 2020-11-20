#include <json-c/json.h>
#include "lib.h"

size_t get_json_options_length(char* buffer, char* key)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *options; /* json object options */
	size_t n_options;

	parsed_json = json_tokener_parse(buffer);
    json_object_object_get_ex(parsed_json, key, &options);

	n_options = json_object_array_length(options);

    return n_options;
}

char** get_json_array_data(char** data, char* key, char* buffer)
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

    return data;
}

char* get_json_data(char* key, char* buffer)
{
	struct json_object *parsed_json; /* json object file */
	struct json_object *data; /* json object data */
	char* str;

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, key, &data);
	str = (char*)json_object_get_string(data);

    return str;
}
