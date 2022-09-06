#include <qcommon.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <qresponder.h>
#include <qstr.h>
#include <jsmn.h>

int enable_debug = 0;
QlicState qlic_state;
QlicConfig qlic_config;

char* qlic_chat_id;

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

static struct curl_slist* __qlic_set_request_headers(QlicContext* context, qstr access_token) {
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

QlicContext* qlic_context_access_init(qstr access_token) {
	QlicContext* qlic_context = qlic_init();
	if (qlic_context) {
		CURL* curl = (CURL*)qlic_context->context;
		struct curl_slist* list = __qlic_set_request_headers(qlic_context, access_token);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		return qlic_context;
	} else {
		return NULL;
	}
}


QlicContext* qlic_init() {
	CURL *curl = curl_easy_init();
	if (curl) {
		QlicContext* qlic_context = (QlicContext*)calloc(1, sizeof(QlicContext));
		qlic_context->context = curl;
		return qlic_context;
	} else {
		return NULL;
	}
}

void qlic_request(QlicContext* context, qlic_response_callback callback, bool is_post_request, qstr payload) {
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
		curl_easy_cleanup(curl);
	}
}

