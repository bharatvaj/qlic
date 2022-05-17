#ifndef __CLIQ_API_H
#define __CLIQ_API_H

#include <qlic_common.h>

QlicString* qlic_send_message_str(QlicString* chat_id);
QlicString* qlic_send_files_str(QlicString* chat_id);

#endif
