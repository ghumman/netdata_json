#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsmn.h"
#include "json.h"

#define JSON_TOKENS 1024

int json_tokens = JSON_TOKENS;

// a json string for testing
// char TEST_JSON_STRING[400] = "{\"name\": [\"element1\":\"ele1\",\"element2\":\"ele2\",\"element3\":\"ele3\",\"element4\":\"ele4\",\"element5\":\"ele5\",\"element6\":\"ele6\",\"element7\":\"ele7\",\"element8\":\"ele8\"], \"class\": 5 , \"marks\": \"1000\", \"groups\": \"users\"}";
char TEST_JSON_STRING[400] = "{\"name1\": \"element1\", \"name2\": \"element2\"}";


// main function also used for testing
int main() {
		
	// call json_test function, we can check resultOfTest for 
	// JSON_OK, or JSON_CANNOT_DOWNLOAD or JSON_CANNOT_PARSE
	int resultOfTest = json_test(TEST_JSON_STRING);

	return EXIT_SUCCESS;
}

// calls json_parse internal json parser
int json_test(char *str)
{
	return json_parse(str, NULL, json_callback_print);
}

// json_parse gets string and callback function and sends it to
// function json_tokenise and receives tokens
int json_parse(char *js, void *callback_data, int (*callback_function)(JSON_ENTRY *))
{
	size_t count;	// count will keep track of total number of json string elements.
	// if string is not null
	if(js) {
		// get the tokens. it calls json_tokenise which will call jsmn_parse from parsing library
		jsmntok_t *tokens = json_tokenise(js, strlen(js), &count);
		// if tokens are not null
		if(tokens) {
			// based on tokens->type it this function calls ...walk_object/array/string/primitive...
			json_walk_tree(js, tokens, callback_data, callback_function);
			// freez(tokens);
		 	free(tokens);
		 	return JSON_OK;
		}
	
		// else if tokens are null
		return JSON_CANNOT_PARSE;
	}
	// if string is null
	return JSON_CANNOT_DOWNLOAD;
}

// callback function passed to json_walk_tree
int json_callback_print(JSON_ENTRY *e)
{
    // BUFFER *wb=buffer_create(300);
    char *wb = malloc(sizeof(char) * 300);

    // buffer_sprintf(wb,"%s = ", e->name);
    sprintf(wb,"%s = ", e->name);
    char txt[50];
	switch(e->type) {
		case JSON_OBJECT:
			e->callback_function = json_callback_print;
			// buffer_strcat(wb,"OBJECT");
			strcat(wb,"OBJECT");
			printf("case: JSON_OBJECT, wb: %s\n", wb);
			break;

		case JSON_ARRAY:
			e->callback_function = json_callback_print;
			sprintf(txt,"ARRAY[%lu]", e->data.items);
			// buffer_strcat(wb, txt);
			strcat(wb, txt);
			printf("case: JSON_ARRAY, wb: %s\n", wb);
			break;

		case JSON_STRING:
			// buffer_strcat(wb, e->data.string);
			strcat(wb, e->data.string);
			printf("case: JSON_STRING, wb: %s\n", wb);
			break;

		case JSON_NUMBER:
            		sprintf(txt,"%Lf", e->data.number);
			// buffer_strcat(wb,txt);
			strcat(wb,txt);
			printf("case: JSON_NUMBER, wb: %s\n", wb);

			break;

		case JSON_BOOLEAN:
			// buffer_strcat(wb, e->data.boolean?"TRUE":"FALSE");
			strcat(wb, e->data.boolean?"TRUE":"FALSE");
			printf("case: JSON_BOOLEAN, wb: %s\n", wb);
			break;

		case JSON_NULL:
			// buffer_strcat(wb,"NULL");
			strcat(wb,"NULL");
			printf("case: JSON_NULL, wb: %s\n", wb);
			break;
	}
	// info("JSON: %s", buffer_tostring(wb)); printf("JSON: %s\n", wb); // buffer_free(wb);
	free(wb);
	return 0;
}

// calls jsmn_parse from jsmn library and returns parse tokens 
jsmntok_t *json_tokenise(char *js, size_t len, size_t *count)
{
	
	int n = json_tokens;
	if(!js || !len) {
		// error("JSON: json string is empty.");
		printf("JSON: json string is empty.");
		return NULL;
	}

	
	jsmn_parser parser;
	jsmn_init(&parser);

	// jsmntok_t *tokens = mallocz(sizeof(jsmntok_t) * n);
	jsmntok_t *tokens = malloc(sizeof(jsmntok_t) * n);
	if(!tokens) return NULL;

	int ret = jsmn_parse(&parser, js, len, tokens, n);
	
	while (ret == JSMN_ERROR_NOMEM) {
		n *= 2;
		// jsmntok_t *new = reallocz(tokens, sizeof(jsmntok_t) * n);
		jsmntok_t *new = realloc(tokens, sizeof(jsmntok_t) * n);
		if(!new) {
			// freez(tokens);
			free(tokens);
			return NULL;
		}
		tokens = new;
		ret = jsmn_parse(&parser, js, len, tokens, n);
	}
	

	if (ret == JSMN_ERROR_INVAL) {
		// error("JSON: Invalid json string.");
		printf("JSON: Invalid json string.");
		// freez(tokens);
		free(tokens);
		return NULL;
	}
	else if (ret == JSMN_ERROR_PART) {
		// error("JSON: Truncated JSON string.");
		printf("JSON: Truncated JSON string.");
		// freez(tokens);
		free(tokens);
		return NULL;
	}

	if(count) *count = (size_t)ret;

	if(json_tokens < n) json_tokens = n;
	return tokens;
}


// depending on JSMN_TYPE it will call associated json_walk_xxxx function
size_t json_walk_tree(char *js, jsmntok_t *t, void *callback_data, int (*callback_function)(struct json_entry *))
{
	JSON_ENTRY e = {
		.name = "",
		.fullname = "",
		.callback_data = callback_data,
		.callback_function = callback_function
	};

	
	switch (t[0].type) {
		case JSMN_OBJECT:
			e.type = JSON_OBJECT;
			printf("e.type is JSON OBJECT\n");
			json_walk_object(js, t, 0, 0, &e);
			break;

		case JSMN_ARRAY:
			e.type = JSON_ARRAY;
			printf("e.type is JSON ARRAY\n");
			json_walk_array(js, t, 0, 0, &e);
			break;

		case JSMN_PRIMITIVE:
			e.type = JSON_NUMBER;
			printf("e.type is JSON PRIMITIVE\n");
			json_walk_primitive(js, t, 0,&e);
			break;
		case JSMN_STRING:
			e.type = JSON_STRING;
			printf("e.type is JSON STRING\n");
			json_walk_string(js, t, 0,&e);
			break;
	}
	
	return 1;
}

// for string json elements types
size_t json_walk_string(char *js, jsmntok_t *t, size_t start, JSON_ENTRY *e)
{
	char old = js[t[start].end];
	js[t[start].end] = '\0';
	e->original_string = &js[t[start].start];

	e->type = JSON_STRING;
	e->data.string = e->original_string;
	if(e->callback_function) e->callback_function(e);
	js[t[start].end] = old;
	return 1;
}

// for primitive json elements types like numbers, boolean or null
size_t json_walk_primitive(char *js, jsmntok_t *t, size_t start, JSON_ENTRY *e)
{
	char old = js[t[start].end];
	js[t[start].end] = '\0';
	e->original_string = &js[t[start].start];

	switch(e->original_string[0]) {
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
		case '8': case '9': case '-': case '.':
			e->type = JSON_NUMBER;
			e->data.number = strtold(e->original_string, NULL);
			break;

		case 't': case 'T':
			e->type = JSON_BOOLEAN;
			e->data.boolean = 1;
			break;

		case 'f': case 'F':
			e->type = JSON_BOOLEAN;
			e->data.boolean = 0;
			break;

		case 'n': case 'N':
		default:
			e->type = JSON_NULL;
			break;
	}
	if(e->callback_function) e->callback_function(e);
	js[t[start].end] = old;
	return 1;
}

// for array json elements types 
size_t json_walk_array(char *js, jsmntok_t *t, size_t nest, size_t start, JSON_ENTRY *e)
{
	JSON_ENTRY ne = {
			.name = "",
			.fullname = "",
			.callback_data = NULL,
			.callback_function = NULL
	};

	char old = js[t[start].end];
	js[t[start].end] = '\0';
	ne.original_string = &js[t[start].start];

	memcpy(&ne, e, sizeof(JSON_ENTRY));
	ne.type = JSON_ARRAY;
	ne.data.items = t[start].size;
	ne.callback_function = NULL;
	ne.name[0]='\0';
	ne.fullname[0]='\0';
	if(e->callback_function) e->callback_function(&ne);
	js[t[start].end] = old;

	size_t i, init = start, size = t[start].size;

	start++;
	for(i = 0; i < size ; i++) {
		ne.pos = i;
		if (!e->name || !e->fullname || strlen(e->name) > JSON_NAME_LEN  - 24 || strlen(e->fullname) > JSON_FULLNAME_LEN -24) {
		    // info("JSON: JSON walk_array ignoring element with name:%s fullname:%s",e->name, e->fullname);
		    printf("JSON: JSON walk_array ignoring element with name:%s fullname:%s",e->name, e->fullname);
		    continue;
		}
		sprintf(ne.name, "%s[%lu]", e->name, i);
		sprintf(ne.fullname, "%s[%lu]", e->fullname, i);

		switch(t[start].type) {
			case JSMN_PRIMITIVE:
				start += json_walk_primitive(js, t, start, &ne);
				break;

			case JSMN_OBJECT:
				start += json_walk_object(js, t, nest + 1, start, &ne);
				break;

			case JSMN_ARRAY:
				start += json_walk_array(js, t, nest + 1, start, &ne);
				break;

			case JSMN_STRING:
				start += json_walk_string(js, t, start, &ne);
				break;
		}
	}
	return start - init;
}

// for object json elements types 
size_t json_walk_object(char *js, jsmntok_t *t, size_t nest, size_t start, JSON_ENTRY *e)
{
	JSON_ENTRY ne = {
		.name = "",
		.fullname = "",
		.callback_data = NULL,
		.callback_function = NULL
	};

	char old = js[t[start].end];
	js[t[start].end - 1] = '\0';
	ne.original_string = &js[t[start].start];
	memcpy(&ne, e, sizeof(JSON_ENTRY));
	ne.type = JSON_OBJECT;
	ne.callback_function = NULL;
	if(e->callback_function) e->callback_function(&ne);
	js[t[start].end] = old;

	int key = 1;
	size_t i, init = start, size = t[start].size;

	start++;
	for(i = 0; i < size ; i++) {
		switch(t[start].type) {
			case JSMN_PRIMITIVE:
				start += json_walk_primitive(js, t, start, &ne);
				key = 1;
				break;

			case JSMN_OBJECT:
				start += json_walk_object(js, t, nest + 1, start, &ne);
				key = 1;
				break;

			case JSMN_ARRAY:
				start += json_walk_array(js, t, nest + 1, start, &ne);
				key = 1;
				break;

			case JSMN_STRING:
			default:
				if(key) {
					int len = t[start].end - t[start].start;
					// if (unlikely(len>JSON_NAME_LEN)) len=JSON_NAME_LEN;
					if (!(len>JSON_NAME_LEN)) len=JSON_NAME_LEN;
					strncpy(ne.name, &js[t[start].start], len);
					ne.name[len] = '\0';
					len=strlen(e->fullname) + strlen(e->fullname[0]?".":"") + strlen(ne.name);
					// char *c = mallocz((len+1)*sizeof(char));
					char *c = malloc((len+1)*sizeof(char));
					sprintf(c,"%s%s%s", e->fullname, e->fullname[0]?".":"", ne.name);
					// if (unlikely(len>JSON_FULLNAME_LEN)) len=JSON_FULLNAME_LEN;
					if (!(len>JSON_FULLNAME_LEN)) len=JSON_FULLNAME_LEN;
					strncpy(ne.fullname, c, len);
					// freez(c);
					free(c);
					start++;
					key = 0;
				}
				else {
					start += json_walk_string(js, t, start, &ne);
					key = 1;
				}
				break;
		}
	}
	return start - init;
}
