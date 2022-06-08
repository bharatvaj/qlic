#ifndef __QLIC_OAUTH_H
#define __QLIC_OAUTH_H

#include <oauth2.h>
#include <stdio.h>
#include <config.h>
#include <nxjson.h>

#include "qlic_types.h"
#include "qlic_private.h"

// TODO Choose between DB and text files for saving this information
// If using text, choose between formats, yaml or json or other format, which is more suckless
// If DB, sliqte3 is a good choice, but don't
// Going to need a json parser and writer, might as well make the config files as json as well
const char* saved_grant_token = NULL;
const char* saved_access_token = NULL;
/* const char* saved_grant_token = ""; */

/* const char* saved_access_token = ""; */

// TODO rethink this decision later, char* or const char* ?
// Have to free the returning string at the end, who does that?
char *json_access_code_transformer(char* str) {
	const nx_json* json = nx_json_parse(str, nx_json_unicode_to_utf8);
	if (json->type == NX_JSON_OBJECT) {
		const nx_json* at = nx_json_get(json, "access_token");
		if (at->type == NX_JSON_STRING) {
			printf("str: %s\n", str);
			const char* value = at->text_value;
			size_t len = strlen(value);
			char* copy = (char*)malloc(len);
			strncpy(copy, value, len);
			printf("copy: %s\n", copy);
			return copy;
		}
	}
	return NULL;
}


/*
 * returns >1 if the file is sucessfully read
 * returns the err value (<=0) in case of errors
 * return -2 if dest is NULL
 * returs -3 if dest_len is NULL
 */
uint8_t read_contents(char* dest, size_t* dest_len, FILE* fp) {
	if (dest == NULL) {
		return -2;
	}
	if (dest_len == NULL) {
		return -3;
	}
	int err = -1;
	size_t res = *dest_len;
	while ((res = fread(dest, 1, QLIC_FILE_BUFFER_SIZE, fp)) > 0) {
		*dest_len += res;
		if (res == 0) {
			if((err = ferror(fp)) != 0) {
				// error occured
				return err;
			} else if((err = feof(fp)) != 0) {
				// return okay
				return err;
			}
			/// unknown error
			return -1;
		} else {
			// res > QLIC_FILE_READ_SIZE
			dest = realloc(dest, *dest_len + QLIC_FILE_BUFFER_SIZE);
		}
	}
	return -1;
}

QlicErrorCode qlic_read_config_file(QlicContext* ctx) {
	/// @todo use xdg paths
	FILE* fp = fopen("config.json", "r");
	char* dest = malloc(sizeof(char) * QLIC_FILE_BUFFER_SIZE);
	size_t dest_len = QLIC_FILE_BUFFER_SIZE;
	read_contents(dest, &dest_len, fp);
	const nx_json* json = nx_json_parse(dest, nx_json_unicode_to_utf8);
	if (json->type == NX_JSON_OBJECT) {
		const nx_json* at_user = nx_json_get(json, "user_id");
		if (at_user == NULL) return QLIC_ERROR;
		if (at_user->type == NX_JSON_OBJECT) {
			const nx_json* at_client_id = nx_json_get(at_user, "client_id");
			if (at_client_id == NULL) return QLIC_ERROR;
			if (at_client_id->type == NX_JSON_STRING) {
				qlic_error(at_client_id->text_value);
				/// @todo maybe nxjson already string length has it?
				__QLIC_ASSIGN_STRING(ctx->client_id, at_client_id->text_value);
				qlic_error(ctx->client_id->string);
				if (at_client_id->text_value == NULL) {
					return QLIC_ERROR;
				}
			}
			const nx_json* at_client_secret = nx_json_get(at_user, "client_secret");
			if (at_client_secret == NULL) return QLIC_ERROR;
			if (at_client_secret->type == NX_JSON_STRING) {
				qlic_error(at_client_secret->text_value);
				/// @todo maybe nxjson already string length has it?
				__QLIC_ASSIGN_STRING(ctx->client_secret, at_client_secret->text_value);
				qlic_error(ctx->client_secret->string);
				if (at_client_secret->text_value == NULL) {
					return QLIC_ERROR;
				}
			}
		}
	}
	return QLIC_ERROR;
}

char* start_oauth_server(QlicContext* ctx) {
	int err = -1;
	err = qlic_read_config_file(ctx);
	if (err) {
		qlic_error("Not able to read config file");
		QLIC_PANIC();
	}

	oauth2_config* conf = oauth2_init(ctx->client_id->string, ctx->client_secret->string);
	if (conf == NULL) {
		qlic_error("conf is null\n");
		QLIC_PANIC();
	}
	conf->access_auth_code_transformer = json_access_code_transformer;
    oauth2_set_redirect_uri(conf, CLIQ_REDIRECT_URI);
	// TODO generate true state instead of LOL
	char* redir_uri = NULL;
	char* grant_token = malloc(255 * sizeof(char));
	if (saved_grant_token == NULL) {
		redir_uri = oauth2_request_auth_code(conf, CLIQ_AUTH_ENDPOINT, CLIQ_SCOPE, "LOL", "online");

		printf("Visit this url and hit authorize: %s\n", redir_uri);
		printf("Now put the auth token here: ");

		scanf("%s", grant_token);
	} else {
		strcpy(grant_token, saved_grant_token);
	}

	if (grant_token == NULL) {
		printf("grant_token is null");
		return NULL;
	}

    //Now test token based auth
    char* access_token = NULL;
	if (saved_access_token == NULL) {
		access_token = oauth2_access_auth_code(conf, CLIQ_TOKEN_ENDPOINT, grant_token, CLIQ_SCOPE);
		if (access_token == NULL) {
			printf("access_token: %ld is null\n", (long)access_token);
			return NULL;
		}
	} else {
		access_token = malloc(255 * sizeof(char));
		strcpy(access_token, saved_access_token);
	}

    oauth2_set_auth_code(conf, access_token);
    printf("Access Token: %s\n", access_token);
    /* free(access_token); */

    /* printf("Enter your Facebook status: "); */
    /* char status[255]; */
    /* scanf("%s", status); */
    /* char status2[255]; */
    /* sprintf(status2, "message=%s", status); */

    /* access_token = oauth2_request(conf, "https://graph.facebook.com/slugonamission/feed", status2); */

    /* printf("%s\n", access_token); */

    oauth2_cleanup(conf);
	return access_token;
}
#endif
