#include <qoauth.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <qtypes.h>
#include <qcommon.h>
#include <capis.h>
#include <jsmn.h>

// TODO Have to free the returning string at the end, who does that?
char *json_access_code_transformer(char* str) {
	jsmn_parser oauth_parser;
	struct jsmntok oauth_tokens[10];
	int oauth_tok_count = jsmn_parse(&oauth_parser, str, strlen(str), oauth_tokens, 10);
	qstr atoken = get_val(str, "access_token", oauth_tokens, oauth_tok_count);
	qstr rtoken = get_val(str, "refresh_token", oauth_tokens, oauth_tok_count);
	memcpy(qlic_state.access_token, atoken, qstrlen(atoken));
	memcpy(qlic_state.refresh_token, atoken, qstrlen(rtoken));
	// TODO convert to UNIX timestamp SAFELY
	/* qlic_state.expires_in = get_val(str, "expires_in", oauth_tokens, oauth_tok_count); */
	return NULL;
}

char* start_oauth_server(qlicconfig_t* qlic_config) {
	/* int err = -1; */

	oauth2_config* conf = oauth2_init(qlic_config->client_id, qlic_config->client_secret);
	if (conf == NULL) {
		inform("FATAL: Failed to create oauth object\n");
		return NULL;
	}
	conf->access_auth_code_transformer = json_access_code_transformer;
    oauth2_set_redirect_uri(conf, CLIQ_REDIRECT_URI);
	// TODO generate true state instead of LOL
	char* redir_uri = NULL;
	char* grant_token = malloc(255 * sizeof(char));
	redir_uri = oauth2_request_auth_code(conf, CLIQ_AUTH_ENDPOINT, CLIQ_SCOPE, "LOL", "offline");

	printf("Visit this url and hit authorize: %s\n", redir_uri);
	printf("Now put the auth token here: ");

	// FIXME this will overflow if input is greater than 255!
	scanf("%s", grant_token);

	if (grant_token == NULL) {
		printf("FATAL: grant_token is null");
		return NULL;
	}

    //Now test token based auth
    char* access_token = oauth2_access_auth_code(conf, CLIQ_TOKEN_ENDPOINT, grant_token, CLIQ_SCOPE);
	if (access_token == NULL) {
		printf("access_token: %ld is null\n", (long)access_token);
		return NULL;
	}

    oauth2_set_auth_code(conf, access_token);
    printf("Access Token: %s\n", access_token);
    /* free(access_token); */
    oauth2_cleanup(conf);
	return access_token;
}
