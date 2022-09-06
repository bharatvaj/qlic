#ifndef __QLIC_TYPES_H
#define __QLIC_TYPES_H

#include <stddef.h>
#include <qstr.h>

typedef size_t (*qlic_response_callback)(char*, size_t, size_t, void*);

// data structure of json
typedef struct {
	/* server data */
	qstr grant_token;
	qstr access_token;
	qstr refresh_token;
	qstr expires_in;
	/* client data */
	qstr time_generated;
} QlicState;

typedef struct {
	qstr client_id;
	qstr client_secret;
} QlicConfig;


typedef enum QlicErrorCode {
	QLIC_ERROR = 0,
	QLIC_OK
} QlicErrorCode;

struct QlicCliqAction {
	char* request_url;
	size_t request_url_len;
	qlic_response_callback callback;
};

/**
 * Holds network information for the current request
 */
typedef struct QlicContext {
	/* cURL ctx  */
	void* context;
	qstr request_url;
} QlicContext;

#endif
