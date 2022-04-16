#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qlic_common.h>
#include <cliq_apis.h>
#include <qlic_response_handler.h>


// TODO Send error back
#define __QLIC_ACCESS_TOKEN "Zoho-oauthtoken "
int main(int argc, char* argv[]) {
	QlicString* access_token = init_qlic_string();
	__QLIC_ASSIGN_STRING(access_token, __QLIC_ACCESS_TOKEN);
	if (argc == 1) {
		qlic_error("Not enough arguments");
		return -1;
	}
	if (strcmp(argv[1], "-r") == 0) {
		QlicContext* qlic_context = qlic_context_access_init(access_token);
		if (qlic_context == NULL) {
			qlic_error("Cannot init network library");
			return -1;
		}
		QlicString* chat_id = init_qlic_string();
		__QLIC_ASSIGN_STRING(chat_id, "2243227993181997558");
		qlic_context->request_url = qlic_send_message_str(chat_id);
		qlic_request(qlic_context, qlic_handle_send_message, true);
	}
	return 0;
}

