#ifndef __QLIC_TYPES_H
#define __QLIC_TYPES_H

#include <stddef.h>
#include <qstr.h>
#include <time.h>

typedef size_t (*qlic_response_callback)(char*, size_t, size_t, void*);

// 128 so it aligns properly
// TODO make the struct packed
/* attribute(__packed__) */
typedef struct qlicstate {
	char grant_token[128];
	char access_token[128];
	char refresh_token[128];
	time_t time_generated;
} qlicstate_t;

typedef struct qlicconfig {
	qstr client_id;
	qstr client_secret;
} qlicconfig_t;


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
typedef struct qliccontext {
	/* cURL ctx  */
	void* context;
	qstr request_url;
} qliccontext_t;

#endif
