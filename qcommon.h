#ifndef __QLIC_COMMON_H
#define __QLIC_COMMON_H

#include <qtypes.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

typedef struct jsmntok jsmntok_t;

#define QLIC_FILE_BUFFER_SIZE 256
/// @todo this can be optimized based on the cpu

/* #define LOAD_INTO_STRUCT(STRUCT, FIELD, JSON) \ */
/* 		const nx_json* __LINE__##FIELD = nx_json_get(JSON, #FIELD); \ */
/* 		if (__LINE__##FIELD->type == NX_JSON_STRING) { \ */
/* 			STRUCT.FIELD = qstrnew(__LINE__##FIELD->text_value); \ */
/* 		} else { \ */
/* 			inform("refresh not able to obtain\n"); \ */
/* 		} */
/*  */

qstr get_val(const char* str, const char* constant, jsmntok_t* tokens, int tokens_size);

extern int enable_debug;

/* Mutable global values */
extern QlicState qlic_state;
extern QlicConfig qlic_config;

extern char* qlic_chat_id;

#define QLIC_PANIC() \
	fprintf(stderr, "qlick panicked at %s:%d", __FILE__, __LINE__); \
	exit(-1);

size_t debug(const char *format, ...);

#define debug(...) \
if (enable_debug) { \
  fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
}


size_t inform(const char *format, ...);

void qlic_error(const char* error_message);

QlicContext* qlic_context_access_init(qstr access_token);

QlicContext* qlic_init();

void qlic_request(QlicContext* context, qlic_response_callback callback, bool is_post_request, qstr payload);

#endif
