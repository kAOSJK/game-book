#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

char* replaceWord(const char* s, const char* oldW, const char* newW) 
{ 
    char* result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 

    for (i = 0; s[i] != '\0'; i++)
    { 
        if (strstr(&s[i], oldW) == &s[i])
        { 
            cnt++; 
            // Jumping to index after the old word. 
            i += oldWlen - 1;
        } 
    } 
  
    // Making new string of enough length 
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) 
    {
        if (strstr(s, oldW) == s)
        { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
    return result; 
}

char** sentence_separator(char* str, char* separator)
{
	char** array = malloc(sizeof(char*));
    char** tmp = NULL;
    char* token = NULL;
    size_t n;

	if (array)
	{
        n = 1;
		token = strtok(str, separator);
		while (token)
		{
			tmp = realloc(array, (n + 1) * sizeof(char*));
			if (tmp == NULL) break;

			array = tmp;
			++n;

			array[n - 2] = malloc(strlen(token) + 1);
			if (array[n - 2] != NULL) strcpy(array[n - 2], token);
			token = strtok(NULL, separator);
		}

		array[n - 1] = NULL;
	}

	return array;
}

char* int_to_word(int n)
{
    if (n == 1) return "first";
    else if (n == 2) return "second";
    else if (n == 3) return "third";
    else if (n == 4) return "fourth";
    else if (n == 5) return "fifth";
    else if (n == 6) return "sixth";
    else if (n == 7) return "seventh";
    else if (n == 8) return "eigth";
    else if (n == 9) return "ninth";
    else if (n == 10) return "tenth"; 
}

void update_json(char* temp, FILE* out)
{
    out = fopen("data/data2.json", "w");
    fputs(temp, out);
    free(temp);
    fclose(out);
}
