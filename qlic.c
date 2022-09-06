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

static void qlic_usage() {
	fputs("usage: qlic [-var] [-c chat_id] [-s msg]\n", stderr);
	exit(1);
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

qstr read_file(FILE *fp) {
	/**
	 * Returns a buffer filled with contents from fp
	 * Works only for files upto 1GB
	 */
	const size_t buf_size = 64;
	qstr buffer = qstrmalloc(buf_size);
	size_t total_bytes_read = 0;
	size_t cbr = 0;
	while ((cbr = fread(buffer + total_bytes_read, 1, buf_size, fp)) > 0) {
		if (cbr > 0) {
			total_bytes_read += cbr;
			size_t newsize = total_bytes_read + buf_size;
			debug("buffer: %p, newsize: %ld, ", buffer, newsize);
			buffer = qstrrealloc(buffer, newsize);
			debug("buffer after qstrrealloc: %p\n", buffer);
		} else {
			// cbr is -1 or 0 when error occurs or during eof
			break;
		}
	}
	return buffer;
}

qstr get_file(int filetype) {
	FILE *fp = NULL;
	qstr path_val = qstrnew(getenv(qlic_env_vars[filetype]));
	if (path_val == NULL) {
		path_val = qstrnew(qlic_env_default_vars[filetype]);
		/* TODO check if this is alright */
		/* append home path */
		qstr home_path = qstrnew(getenv("HOME"));
		if (home_path != NULL) {
			qstrsprintf(&path_val, "%s/%s", home_path, path_val);
		}
	}
	debug("%s file: %s\n", qlic_file_types[filetype], path_val);
	fp = fopen(path_val, qlic_file_flags[filetype]);
	if (fp == NULL) {
		inform("Not able to open %s\n", path_val);
		return NULL;
	}
	qstr file_contents = read_file(fp);
	fclose(fp);
	/* qstrfree(path_val); */
	return file_contents;
}

/* Read from struct */
void load_state(jsmntok_t* token, int size) {
		qlic_state.access_token = get_val(state_jstr, "access_token", token, size);
		qlic_state.grant_token = get_val(state_jstr, "grant_token", token, size);
		fprintf(stderr, "State: %s\n", qlic_state.access_token);
		fprintf(stderr, "State: %s\n", qlic_state.grant_token);
}

static void load_config(jsmntok_t* token, int size) {
		qlic_config.client_id = get_val(config_jstr, "client_id", token, size);
		qlic_config.client_secret = get_val(config_jstr, "client_secret", token, size);
		fprintf(stderr, "Config: %s\n", qlic_config.client_id);
		fprintf(stderr, "Config: %s\n", qlic_config.client_secret);
}

int main(int argc, char *argv[])
{
	jsmntok_t config_tokens[40];
	jsmntok_t state_tokens[40];
	jsmn_init(&config_json);
	jsmn_init(&state_json);
	if (argc == 1) {
		qlic_usage();
		return -1;
	}
	int i = 1;
	if (!strcmp(argv[i], "-d")) {
		i++;
		enable_debug = 1;
	}

	state_jstr = get_file(STATE_FILE);
	config_jstr = get_file(CONFIG_FILE);
	int config_tok_count = jsmn_parse(&config_json, config_jstr, qstrlen(config_jstr), config_tokens, 40);
	int state_tok_count = jsmn_parse(&state_json, state_jstr, qstrlen(state_jstr), state_tokens, 40);

	/* QlicContext *ctx = qlic_init(); */
	if (state_tok_count < 0) {
		inform("Cannot parse state.json\n");
		return -1;
	}
	if (config_tok_count < 0) {
		// TODO if config is found load the global states from the json
	}

	load_state(state_tokens, state_tok_count);
	load_config(config_tokens, config_tok_count);
	debug("access_token: %s\n", qlic_state.access_token);
	if (qlic_state.access_token == NULL) {
		inform("Access token not found, try auth again\n");
		exit(-1);
	}
	// TODO Use an argument parsing library or not, make a decision!
	/* inform("Value of i is: %d\n", i); */
	/* inform("Value of argv[i] is: %s\n", argv[i]); */
	inform("Value of argc is: %d\n", argc);
	for (; i < argc && argv[i][0] == '-'; i++) {
		inform("Value of i is: %d\n", i);
		inform("Value of argv[i] is: %s\n", argv[i]);
		for (int j = 1; argv[i][j] != '\0'; j++) {
			inform("Value of argv[%d][%d] is: %c\n", i, j, argv[i][j]);
			int f = argv[i][j];
			if (f == 'v') {
				fputs("qlic v" QLIC_VERSION "\n", stderr);
			} else if (f == 'd') {
				enable_debug = 1;
			} else if (f == 'c') {
				if (i + 1 < argc) {
					i = i + 1;
					qlic_chat_id = argv[i];
					break;
				}
			} else if (f == 'r') {
				qlic_recv_msg(qlic_state.access_token);
			} else if (f == 's') {
				if (i + 1 < argc) {
					qlic_send_text_msg(qlic_state.access_token, argv[++i]);
				} else {
					qlic_usage();
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
				/* qlic_usage(); */
			}
		}
	}
	return 0;
}
