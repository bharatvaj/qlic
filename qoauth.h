#ifndef __QLIC_OAUTH_H
#define __QLIC_OAUTH_H

#include <oauth2.h>
#include <stdio.h>
#include <config.h>
#include <string.h>
#include <stdlib.h>
#include <nxjson.h>

#include <qtypes.h>

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

char* start_oauth_server() {
	/* int err = -1; */

	oauth2_config* conf = oauth2_init(qlic_config.client_id, qlic_config.client_secret);
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
