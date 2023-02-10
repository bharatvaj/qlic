#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>

#include <capis.h>
#include <qcommon.h>
#include <qoauth.h>
#include <qresponder.h>

#include "config.h"

#include <jsmn.h>

int recv_requested = 0;
int send_requested = 0;
int auth_requested = 0;

jsmn_parser config_json;

qstr config_jstr = NULL;

int sig_rcvd = 0;

void qlic_sig_hdlr(int sig) {
	if (recv_requested) {
		sig_rcvd = 1;
	} else {
		exit(sig);
	}
}

static int qlic_usage() {
	fputs("usage: qlic [-va] [-s msg |-r <chat_id> [chat_id..]]\n", stderr);
	return -1;
}

int qlic_recv_msg(const qstr access_token) {
	while(!sig_rcvd) {
		// TODO use 100ns instead of 1000ns
		sleep(1);
		inform("%s will be used to retreive msg\n", access_token);
	}
	return -1;
}

int qlic_send_text_msg(const qstr access_token, const qstr msg) {
	qliccontext_t *qlic_context = qlic_context_access_init(access_token);
	if (qlic_context == NULL) {
		qlic_error("Cannot init network library");
		return -1;
	}
	qstr payload = NULL;
	qstrsprintf(&payload, "{\"text\": \"%s\"}", msg);
	inform("Sending message: %s\n", msg);
	for (int i = 0; i < chat_ids_len; i++) {
		char* qlic_chat_id = chat_ids[i];
		qlic_context->request_url = qlic_send_message_str(qlic_chat_id);
		qlic_request(qlic_context, qlic_handle_send_message, true, payload);
	}
	qstrfree(payload);
	return 0;
}

static void load_config(jsmntok_t* token, int size) {
		qlic_config.client_id = get_val(config_jstr, "client_id", token, size);
		qlic_config.client_secret = get_val(config_jstr, "client_secret", token, size);
		debug("config.json: client_id: %s\n", qlic_config.client_id);
		debug("config.json: client_secret: %s\n", qlic_config.client_secret);
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		return qlic_usage();
	}

	int i = 0;
	char* send_param = NULL;

	jsmntok_t config_tokens[40];
	jsmn_init(&config_json);

	qstr home_path = qstrnew(getenv("HOME"));
	char* config_path = qstrnew(qlic_env_default_vars[0]);

	/* Parse config.json */
	if (home_path != NULL) {
		qstrsprintf(&config_path, "%s/%s", home_path, config_path);
	}
	FILE* config_fp = fopen(config_path, qlic_file_flags[0]);
	if (config_fp == NULL) {
		inform("Not able to open %s\n", config_path);
		return -1;
	}
	config_jstr  = read_file(config_fp);
	fclose(config_fp);
	int config_tok_count = jsmn_parse(&config_json, config_jstr, qstrlen(config_jstr), config_tokens, 40);
	/* Validate json */
	if (config_tok_count < 2) {
		inform("config.json is invalid\n");
	}

	/* Parse .cache/qlic_state */
	// TODO Append HOME path before opening
	char* state_path = qstrnew(qlic_env_default_vars[1]);
	if (home_path != NULL) {
		qstrsprintf(&state_path, "%s/%s", home_path, state_path);
	}
	FILE* state_fp = fopen(state_path, "r+");
	if (state_fp == NULL) {
		printf("FATAL: Cannot open file\n");
		return -1;
	}
	int bytes_read = fread((void*)&qlic_state, sizeof(qlicstate_t), 1, state_fp);
	if (bytes_read == 0) {
		inform("Cannot read file: %s. Run `qlic -a` first.\n", "$HOME/.cache/qlic_state");
		// TODO use documented status codes
		return -2;
	}
	fclose(state_fp);

	debug("qlic_state: access_token: %s\n", qlic_state.access_token);
	debug("qlic_state: grant_token: %s\n", qlic_state.grant_token);

	load_config(config_tokens, config_tok_count);
	debug("access_token: %s\n", qlic_state.access_token);
	if (strcmp(qlic_state.access_token, "") == 0) {
		inform("Access token not found, try auth again\n");
		exit(-1);
	}

	signal(SIGINT, qlic_sig_hdlr);

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-a")) {
			auth_requested = 1;
			break;
		} else if (!strcmp(argv[i], "-v")) {
			fputs("qlic v" QLIC_VERSION "\n", stderr);
			return 0;
		} else if (!strcmp(argv[i], "-l")) {
			enable_debug = 1;
		} else if (!strcmp(argv[i], "-r")) {
			recv_requested = 1;
			break;
		} else if (!strcmp(argv[i], "-s")) {
			if (i + 1 < argc) {
				send_requested = 1;
				send_param = argv[++i];
				break;
			} else {
				return qlic_usage();
			}
		}
	}

	if (i == argc) {
		return qlic_usage();
	}
	chat_ids = &argv[i];
	chat_ids_len = (argc - i) - 1;

	int errstatus = 0;

	if (auth_requested) {
		char *access_token = start_oauth_server(&qlic_config);
		if (access_token == NULL) {
			qlic_error("Access token is empty, authentication failed");
				errstatus = -1;
		} else {
			int status_status = write_state_file(&qlic_state);
			if (status_status < 0) {
				inform("FATAL: Creation of state file failed\n");
				errstatus = -1;
			}
		}
	} else if (send_requested) {
		errstatus = qlic_send_text_msg(qlic_state.access_token, send_param);
	} else if (recv_requested) {
		errstatus = qlic_recv_msg(qlic_state.access_token);
	} else {
		errstatus = qlic_usage();
	}
	return errstatus;
}
