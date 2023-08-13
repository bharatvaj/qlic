#include <capis.h>
#include <qcommon.h>
#include <stdio.h>
#include <stdlib.h>
#include <qstr.h>

// TODO am I doing basic string substitution here? Should be this moved to config.def.h?
qstr qlic_send_message_str(qstr chat_id) {
	qstr send_message = NULL;
	qstrsprintf(&send_message, "https://cliq.zoho.com/api/v2/chats/%s/message", chat_id);
	return send_message;
}

qstr qlic_recv_message(qstr chat_id) {
	qstr send_message = NULL;
	qstrsprintf(&send_message, "https://cliq.zoho.com/api/v2/chats/%s/message", chat_id);
	return send_message;
}
