#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qcommon.h>
#include <capis.h>
#include <qresponder.h>
#include <qoauth.h>

int qlic_send_text_msg(const qstr access_token, const qstr chat_id) {
	QlicContext* qlic_context = qlic_context_access_init(access_token);
	if (qlic_context == NULL) {
		qlic_error("Cannot init network library");
		return -1;
	}
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
		qstr access_token = qstrnew("1000.11a210512a0aed97a3d4c5a14848d70b.3383057c623f1b59493ab0d8c336eede", sizeof("1000.11a210512a0aed97a3d4c5a14848d70b.3383057c623f1b59493ab0d8c336eede") - 1);
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
