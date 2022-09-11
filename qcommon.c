#include <qcommon.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <qresponder.h>
#include <qstr.h>
#include <jsmn.h>

int enable_debug = 0;
qlicstate_t qlic_state;
qlicconfig_t qlic_config;

char** chat_ids = NULL;
size_t chat_ids_len = 0;

void qlic_error(const char* error_message) {
	fprintf(stderr, "%s\n", error_message);
}

size_t inform(const char *format, ...) {
  va_list va;
  va_start(va, format);
  size_t ret_val = vfprintf(stdout, format, va);
  va_end(va);
  return ret_val;
}

qstr get_val(const char* str, const char* constant, jsmntok_t* tokens, int tokens_size) {
	for(int i = 0; i < tokens_size; i++) {
		jsmntok_t* token = tokens + i;
		if (token->type == JSMN_STRING) {
			if (strncmp(str + token->start, constant, token->end - token->start) == 0) {
				i++;
				jsmntok_t* next_token = tokens + i;
				return qstrnnew(str + next_token->start, next_token->end - next_token->start);
			}
		}
	}
	return NULL;
}

static struct curl_slist* __qlic_set_request_headers(qliccontext_t* context, qstr access_token) {
	__attribute__((unused)) CURL* curl = (CURL*)context->context;
	if(access_token == NULL) {
		return NULL;
	}
	struct curl_slist* list = NULL;
#define __QLIC_AUTHORIZATION_HEADER "Authorization: Zoho-oauthtoken "
	// TODO free up authorization_header if curl doesn't handle
	// TODO check if we instance from qstr lib itself
	qstr authorization_header = qstrnew(__QLIC_AUTHORIZATION_HEADER);
	authorization_header = qstrcat(authorization_header, access_token);
	list = curl_slist_append(list, authorization_header);
	list = curl_slist_append(list, "Content-Type: application/json");
	list = curl_slist_append(list, "contentType: application/json");
	return list;
}
FILE* get_file(int filetype, const char** qlic_env_vars, const char** qlic_env_default_vars, const char** qlic_file_types, const char** qlic_file_flags) {
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
	FILE* fp = fopen(path_val, qlic_file_flags[filetype]);
	if (fp == NULL) {
		inform("Not able to open %s\n", path_val);
		return NULL;
	}
	return fp;
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
	/* qstrfree(path_val); */
	return buffer;
}

int write_state_file(const qlicstate_t* state) {
	FILE* fp = fopen("state.bin", "w+");
	if (fp == NULL) {
		printf("FATAL: Cannot open file\n");
	}
	int bytes_written = fwrite((void*)state, sizeof(qlicstate_t), 1, fp);
	if (bytes_written == 0) {
		printf("FATAL: Cannot write to file\n");
		return -2;
	}

	/* fprintf(fp, "This is working!\n"); */
	return fclose(fp);
}

int read_state_file(qlicstate_t* state) {
	FILE* fp = fopen("/home/laz3r/.cache/qlic_state", "r+");
	if (fp == NULL) {
		printf("FATAL: Cannot open file\n");
		return -1;
	}
	int bytes_read = fread((void*)state, sizeof(qlicstate_t), 1, fp);
	if (bytes_read == 0) {
		printf("FATAL: Cannot read from file\n");
		return -2;
	}
	fclose(fp);
	return 0;
}

qliccontext_t* qlic_context_access_init(qstr access_token) {
	qliccontext_t* qlic_context = qlic_init();
	if (qlic_context) {
		CURL* curl = (CURL*)qlic_context->context;
		struct curl_slist* list = __qlic_set_request_headers(qlic_context, access_token);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		return qlic_context;
	} else {
		return NULL;
	}
}


qliccontext_t* qlic_init() {
	CURL *curl = curl_easy_init();
	if (curl) {
		qliccontext_t* qlic_context = (qliccontext_t*)calloc(1, sizeof(qliccontext_t));
		qlic_context->context = curl;
		return qlic_context;
	} else {
		return NULL;
	}
}

void qlic_request(qliccontext_t* context, qlic_response_callback callback, bool is_post_request, qstr payload) {
	fprintf(stdout, "%s\n", payload);
	if(context) {
		CURL* curl = (CURL*)context->context;
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, context->request_url);
		/* curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0); */
		if (is_post_request) {
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
		}
		/* curl_easy_setopt(curl, CURLOPT_READFUNCTION, qlic_handle_read_chat); */
		/* curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, curl);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
								  curl_easy_strerror(res));
		}
		// TODO handle cleanup elsewhere or allocate curl object in heap
		/* curl_easy_cleanup(curl); */
	}
}

