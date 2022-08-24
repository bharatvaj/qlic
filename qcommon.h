#ifndef __QLIC_COMMON_H
#define __QLIC_COMMON_H

#include <qtypes.h>
#include <stdbool.h>

#define QLIC_FILE_BUFFER_SIZE 256
/// @todo this can be optimized based on the cpu

int enable_debug = 0;

#define QLIC_PANIC() \
	fprintf(stderr, "qlick panicked at %s:%d", __FILE__, __LINE__); \
	exit(-1);

size_t debug(const char *format, ...) {
  if (enable_debug) {
	  va_list va;
	  size_t ret_val = 0;
	  va_start(va, format);
	  ret_val = vfprintf(stderr, format, va);
	  va_end(va);
	  return ret_val;
  }
  return 0;
}

size_t inform(const char *format, ...) {
  va_list va;
  va_start(va, format);
  size_t ret_val = vfprintf(stdout, format, va);
  va_end(va);
  return ret_val;
}

void qlic_error(const char* error_message);

QlicContext* qlic_context_access_init(qstr access_token);

QlicContext* qlic_init();

void qlic_request(QlicContext* context, qlic_response_callback callback, bool is_post_request);

#endif
