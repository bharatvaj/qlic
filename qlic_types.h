#ifndef __QLIC_TYPES_H
#define __QLIC_TYPES_H

#include <stddef.h>

typedef size_t (*qlic_response_callback)(char*, size_t, size_t, void*);

typedef struct QlicString {
	char* string;
	size_t len;
} QlicString;

struct QlicCliqAction {
	char* request_url;
	size_t request_url_len;
	qlic_response_callback callback;
};

/**
 * Network related information
 */
typedef struct QlicContext {
	void* context;
	QlicString* request_url;
} QlicContext;

#endif
