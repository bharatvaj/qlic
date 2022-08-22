#ifndef __QLIC_TYPES_H
#define __QLIC_TYPES_H

#include <stddef.h>
#include <qstr.h>

typedef size_t (*qlic_response_callback)(qstr, size_t, size_t, void*);

struct QlicCliqAction {
	qstr request_url;
	size_t request_url_len;
	qlic_response_callback callback;
};

/**
 * Network related information
 */
typedef struct QlicContext {
	void* context;
	qstr request_url;
} QlicContext;

#endif
