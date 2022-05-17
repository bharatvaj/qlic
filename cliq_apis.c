#include <cliq_apis.h>
#include <qlic_common.h>
#include <stdio.h>
#include <stdlib.h>

QlicString* qlic_send_message_str(QlicString* chat_id) {
#define __QLIC_SEND_MESSAGE_API_STR "https://cliq.zoho.com/api/v2/chats/%s/message"
	QlicString* send_message = init_qlic_string();
	// minus 2 for removing the format specifier :(
	send_message->len = sizeof(__QLIC_SEND_MESSAGE_API_STR) + chat_id->len - 2;
	send_message->string = (char *)malloc(send_message->len * sizeof(char));
	snprintf(send_message->string, send_message->len, __QLIC_SEND_MESSAGE_API_STR, chat_id->string);
	return send_message;
}

QlicString* qlic_send_files_str(QlicString* chat_id) {
#define __QLIC_SEND_FILE_API_STR "https://cliq.zoho.com/api/v2/chats/%s/files"
	QlicString* send_file = init_qlic_string();
	// minus 2 for removing the format specifier :(
	send_file->len = sizeof(__QLIC_SEND_FILE_API_STR) + chat_id->len - 2;
	send_file->string = (char *)malloc(send_file->len * sizeof(char));
	snprintf(send_file->string, send_file->len, __QLIC_SEND_MESSAGE_API_STR, chat_id->string);
	return send_file;
}
