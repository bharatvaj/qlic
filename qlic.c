#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <capis.h>
#include <jsmn.h>
#include <qcommon.h>
#include <qoauth.h>
#include <qresponder.h>

#include "config.h"

jsmn_parser state_json;
jsmn_parser config_json;

qstr config_jstr = NULL;
qstr state_jstr = NULL;

static int qlic_usage() {
	fputs("usage: qlic [-va] [-s msg <chat_id> [chat_id...]] [-r <chat_id> [chat_id..]]\n", stderr);
	return -1;
}

int qlic_recv_msg(const qstr access_token) {
	inform("%s will be used to retreive msg\n", access_token);
	return -1;
}

int qlic_send_text_msg(const qstr access_token, const qstr msg) {
	QlicContext *qlic_context = qlic_context_access_init(access_token);
	if (qlic_context == NULL) {
		qlic_error("Cannot init network library");
		return -1;
	}
	qlic_context->request_url = qlic_send_message_str(qlic_chat_id);
	qstr payload = NULL;
	qstrsprintf(&payload, "{\"text\": \"%s\"}", msg);
	inform("Sending message: %s\n", msg);
	qlic_request(qlic_context, qlic_handle_send_message, true, payload);
	return 0;
}


void load_state() {
		if (read_state_file(&qlic_state) < 0) {
			return;
		}
		debug("qlic_state: access_token: %s\n", qlic_state.access_token);
		debug("qlic_state: grant_token: %s\n", qlic_state.grant_token);
}

static void load_config(jsmntok_t* token, int size) {
		qlic_config.client_id = get_val(config_jstr, "client_id", token, size);
		qlic_config.client_secret = get_val(config_jstr, "client_secret", token, size);
		debug("config.json: client_id: %s\n", qlic_config.client_id);
		debug("config.json: client_secret: %s\n", qlic_config.client_secret);
}

int main(int argc, char *argv[])
{
	jsmntok_t config_tokens[40];
	jsmntok_t state_tokens[40];
	jsmn_init(&config_json);
	jsmn_init(&state_json);
	if (argc == 1) {
		return qlic_usage();
	}
	int i = 1;
	if (!strcmp(argv[i], "-d")) {
		i++;
		enable_debug = 1;
	}

	/* state_jstr = get_file(STATE_FILE); */
	FILE* config_fp = get_file(CONFIG_FILE, qlic_env_vars, qlic_env_default_vars, qlic_file_types, qlic_file_flags);
	config_jstr  = read_file(config_fp);
	fclose(config_fp);
	int config_tok_count = jsmn_parse(&config_json, config_jstr, qstrlen(config_jstr), config_tokens, 40);
	/* int state_tok_count = jsmn_parse(&state_json, state_jstr, qstrlen(state_jstr), state_tokens, 40); */

	int errstatus = read_state_file(&qlic_state);

	/* QlicContext *ctx = qlic_init(); */
	if (errstatus < 0) {
		inform("Cannot parse state.json\n");
		return errstatus;
	}
	if (config_tok_count < 0) {
		// TODO if config is found load the global states from the json
	}

	load_state();
	load_config(config_tokens, config_tok_count);
	debug("access_token: %s\n", qlic_state.access_token);
	if (strcmp(qlic_state.access_token, "") == 0) {
		inform("Access token not found, try auth again\n");
		exit(-1);
	}
	// TODO Use an argument parsing library or not, make a decision!
	/* debug("Value of i is: %d\n", i); */
	/* debug("Value of argv[i] is: %s\n", argv[i]); */
	for (; i < argc && argv[i][0] == '-'; i++) {
		for (int j = 1; argv[i][j] != '\0'; j++) {
			debug("argv[%d][%d]: %c\n", i, j, argv[i][j]);
			int f = argv[i][j];
			if (f == 'v') {
				fputs("qlic v" QLIC_VERSION "\n", stderr);
			} else if (f == 'd') {
				enable_debug = 1;
			} else if (f == 'c') {
				if (i + 1 < argc) {
					qlic_chat_id = argv[++i];
					break;
				}
			} else if (f == 'r') {
				qlic_recv_msg(qlic_state.access_token);
			} else if (f == 's') {
				if (i + 1 < argc) {
					qlic_send_text_msg(qlic_state.access_token, argv[++i]);
					break;
				} else {
					return qlic_usage();
				}
			} else if (f == 'a') {
				char *access_token = start_oauth_server(&qlic_config);
				if (access_token == NULL) {
					qlic_error("Access token is empty, authentication failed");
					return -1;
				} else {
					// TOOD write data to state.json
				}
			} else {
				inform("'%c' is not valid flag\n", argv[i][j]);
				return qlic_usage();
			}
		}
	}
	return 0;
}
