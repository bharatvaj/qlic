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

qstr json_get_val(const char* str, const char* field, jsmntok_t* tokens, int tokens_size) {
	for(int i = 0; i < tokens_size; i++) {
		jsmntok_t* token = tokens + i;
		if (token->type == JSMN_STRING) {
			if (strncmp(str + token->start, field, token->end - token->start) == 0) {
				i++;
				token = tokens + i;
				return qstrnnew(str + token->start, token->end - token->start);
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

