#ifndef __QLIC_COMMON_H
#define __QLIC_COMMON_H

#include <qlic_types.h>
#include <stdbool.h>

#define __QLIC_ASSIGN_STRING(X,Y) \
	X->string = Y; \
	X->len = strlen(Y);

void qlic_error(const char* error_message);

QlicString* init_qlic_string();

void destroy_qlic_string(QlicString*);

QlicContext* qlic_context_access_init(QlicString* access_token);

QlicContext* qlic_init();

void qlic_request(QlicContext* context, qlic_response_callback callback, bool is_post_request);

#endif
