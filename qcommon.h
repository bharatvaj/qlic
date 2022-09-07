#ifndef __QLIC_COMMON_H
#define __QLIC_COMMON_H

#include <qtypes.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

// forward declaration
typedef struct jsmntok jsmntok_t;

/// @todo this can be optimized based on the cpu
#define QLIC_FILE_BUFFER_SIZE 256

qstr get_val(const char* str, const char* constant, jsmntok_t* tokens, int tokens_size);

extern int enable_debug;

/* Mutable global values */
extern qlicstate_t qlic_state;
extern qlicconfig_t qlic_config;

extern char** chat_ids;
extern size_t chat_ids_len;

#define QLIC_PANIC() \
	fprintf(stderr, "qlick panicked at %s:%d", __FILE__, __LINE__); \
	exit(-1);

size_t debug(const char *format, ...);

#define debug(...) \
if (enable_debug) { \
  fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
}

int write_state_file(const qlicstate_t* state);

int read_state_file(qlicstate_t* state);

size_t inform(const char *format, ...);

FILE* get_file(int filetype, const char** qlic_env_vars, const char** qlic_env_default_vars, const char** qlic_file_types, const char** qlic_file_flags);
qstr read_file(FILE *fp);
void qlic_error(const char* error_message);

qliccontext_t* qlic_context_access_init(qstr access_token);

qliccontext_t* qlic_init();

void qlic_request(qliccontext_t* context, qlic_response_callback callback, bool is_post_request, qstr payload);

#endif
