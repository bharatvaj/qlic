#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qlic_common.h>
#include <cliq_apis.h>
#include <qlic_response_handler.h>
#include <qlic_oauth.h>

int qlic_send_text_msg(const char* __access_token, const char* __chat_id) {
	QlicString* access_token = NULL;
	access_token = init_qlic_string();
	access_token->len = strlen(__access_token);
	access_token->string = (char*)malloc(access_token->len * sizeof(__access_token));
	strncpy(access_token->string, __access_token, access_token->len);
	QlicContext* qlic_context = qlic_context_access_init(access_token);
	if (qlic_context == NULL) {
		qlic_error("Cannot init network library");
		return -1;
	}
	QlicString* chat_id = init_qlic_string();
	__QLIC_ASSIGN_STRING(chat_id, __chat_id);
	qlic_context->request_url = qlic_send_message_str(chat_id);
	qlic_request(qlic_context, qlic_handle_send_message, true);
	return 0;
}

// TODO Send error back
int main(int argc, char* argv[]) {
	if (argc == 1) {
		qlic_error("Not enough arguments");
		return -1;
	}
	// TODO Use an argument parsing library
	if (strcmp(argv[1], "-r") == 0) {
		// TODO read access_token from state.json
		char* access_token = "1000.429cf5132d6cc978960bfdd6e0a425cc.80bbd5584b0c35133c4f82143e6811b2";
		// FIXME possible buffer overflow here
		qlic_send_text_msg(access_token, argv[2]);
	} else if (strcmp(argv[1], "-a") == 0) {
		char* access_token = start_oauth_server();
		if (access_token == NULL) {
			qlic_error("Access token is empty, authentication failed");
			return -1;
		}
	}
	return 0;
}

