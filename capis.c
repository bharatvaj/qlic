#include <capis.h>
#include <qcommon.h>
#include <stdio.h>
#include <stdlib.h>

// am I doing basic string substitution here?
qstr qlic_send_message_str(qstr chat_id) {
#define __QLIC_SEND_MESSAGE_API_STR "https://cliq.zoho.com/api/v2/chats/%s/message"
	qstr send_message = NULL;
	qstrsprintf(&send_message, __QLIC_SEND_MESSAGE_API_STR, chat_id);
	return send_message;
}
