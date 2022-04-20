#include <qlic_common.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <qlic_response_handler.h>

void qlic_error(const char* error_message) {
	fprintf(stderr, error_message);
}

static struct curl_slist* __qlic_set_request_headers(QlicContext* context, QlicString* access_token) {
	CURL* curl = (CURL*)context->context;
	if(access_token == NULL) {
		return NULL;
	}
	struct curl_slist* list = NULL;
#define __QLIC_AUTHORIZATION_HEADER "Authorization: Zoho-oauthtoken "
	// TODO free up authorization_header if curl doesn't handle
	size_t authorization_header_len = sizeof(__QLIC_AUTHORIZATION_HEADER) + access_token->len;
	char* authorization_header = (char*)malloc(authorization_header_len);
	strncpy(authorization_header, __QLIC_AUTHORIZATION_HEADER, sizeof(__QLIC_AUTHORIZATION_HEADER));
	strncat(authorization_header, access_token->string, access_token->len);
	list = curl_slist_append(list, authorization_header);
	list = curl_slist_append(list, "Content-Type: application/json");
	list = curl_slist_append(list, "contentType: application/json");
	return list;
}

QlicString* init_qlic_string() {
	QlicString* qlic_string = (QlicString*)malloc(sizeof(QlicString));
	qlic_string->string = NULL;
	qlic_string->len = -1;
	return qlic_string;
}

QlicContext* qlic_context_access_init(QlicString* access_token) {
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

void destroy_qlic_string(QlicString* qlic_string) {
	if(qlic_string->string != NULL) {
		free(qlic_string->string);
	}
	free(qlic_string);
}

QlicContext* qlic_init() {
	CURL *curl = curl_easy_init();
	if (curl) {
		QlicContext* qlic_context = (QlicContext*)malloc(sizeof(QlicContext));
		qlic_context->context = curl;
		return qlic_context;
	} else {
		return NULL;
	}
}

void qlic_request(QlicContext* context, qlic_response_callback callback, bool is_post_request) {
	if(context) {
		CURL* curl = (CURL*)context->context;
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, context->request_url->string);
		/* curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0); */
		if (is_post_request) {
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"text\": \"Hi\"}");
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

