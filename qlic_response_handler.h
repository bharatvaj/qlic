#ifndef __QLIC_RESPONSE_HANDLER_H
#define __QLIC_RESPONSE_HANDLER_H

#include <stddef.h>

int qlic_handle_read_chat(char* response, size_t response_size, size_t nmemb, void *userp);
int qlic_handle_send_message(char* response, size_t response_size, size_t nmemb, void *userp);

#endif
