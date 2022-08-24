#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nxjson.h>
#include <qcommon.h>
#include <qoauth.h>
#include <qresponder.h>
#include <capis.h>

#include "config.h"

const nx_json *state_json = NULL;
const nx_json *config_json = NULL;

static void qlic_usage() {
  fputs("usage: qlic [-va] [-r chat_id] [-s chat_id]\n", stderr);
  exit(1);
}

int qlic_send_text_msg(const qstr access_token, const qstr chat_id) {
  QlicContext *qlic_context = qlic_context_access_init(access_token);
  if (qlic_context == NULL) {
    qlic_error("Cannot init network library");
    return -1;
  }
  qlic_context->request_url = qlic_send_message_str(chat_id);
  qlic_request(qlic_context, qlic_handle_send_message, true);
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

const nx_json *get_json(int filetype) {
  qstr jsonstr = get_file(filetype);
  if (jsonstr == NULL) {
    return NULL;
  }
  /* printf("File contents: %s\n", jsonstr); */
  const nx_json *json = nx_json_parse(jsonstr, nx_json_unicode_to_utf8);
  return json;
}

#define LOAD_INTO_STRUCT(DEST, FIELD, JSON)                                    \
  const nx_json *FIELD = nx_json_get(json_rep, #FIELD);                        \
  if (FIELD->type == NX_JSON_STRING) {                                         \
    DEST.FIELD = qstrnew(FIELD->text_value);                                   \
    debug("%s: %s\n", #FIELD, DEST.FIELD);                           \
  }

void load_state(const nx_json *json_rep) {
  if (json_rep->type == NX_JSON_OBJECT) {
    LOAD_INTO_STRUCT(qlic_state, access_token, json_rep)
    LOAD_INTO_STRUCT(qlic_state, grant_token, json_rep)
  }
}

void load_config(const nx_json *json_rep) {
  if (json_rep->type == NX_JSON_OBJECT) {
    LOAD_INTO_STRUCT(qlic_config, client_id, json_rep)
    LOAD_INTO_STRUCT(qlic_config, client_secret, json_rep)
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
	qlic_usage();
    return -1;
  }

  QlicContext *ctx = qlic_init();
  state_json = get_json(STATE_FILE);
  config_json = get_json(CONFIG_FILE);
  if (state_json == NULL) {
    inform("Cannot parse state.json\n");
    return -1;
  }
  if (config_json != NULL) {
    // TODO if config is found load the global states from the json
  }

  load_state(state_json);
  load_config(config_json);
  debug("access_token: %s\n", qlic_state.access_token);
  if (qlic_state.access_token == NULL) {
    inform("Access token not found, try auth again\n");
    exit(-1);
  }
  // TODO Use an argument parsing library or not, make a decision
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      fputs("qlic v" QLIC_VERSION "\n", stderr);
    } else if (!strcmp(argv[i], "-d")) {
		enable_debug = 1;
	} else if (!strcmp(argv[i], "-r")) {
		  /* ++i as we are going to consume next string  */
      if (++i < argc) {
        qlic_send_text_msg(qlic_state.access_token, argv[i]);
      } else {
		  // print usage and exit
		  /* qlic_usage(); */
	  }
	} else if (!strcmp(argv[i], "-s")) {
		  /* ++i as we are going to consume next string  */
      if (++i < argc) {
        qlic_send_text_msg(qlic_state.access_token, argv[i]);
      } else {
		  // print usage and exit
		  /* qlic_usage(); */
	  }
    } else if (!strcmp(argv[1], "-a")) {
      char *access_token = start_oauth_server(ctx);
      if (access_token == NULL) {
        qlic_error("Access token is empty, authentication failed");
        return -1;
      } else {
        // TOOD write data to state.json
      }
    } else {
      qlic_usage();
    }
  }
  return 0;
}
