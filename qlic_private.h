#ifndef __QLIC_PRIVATE_H
#define __QLIC_PRIVATE_H

#define QLIC_FILE_BUFFER_SIZE 256
/// @todo this can be optimized based on the cpu

#define QLIC_PANIC() \
	fprintf(stderr, "qlick panicked at %s:%d", __FILE__, __LINE__); \
	exit(-1);

#define __QLIC_ASSIGN_STRING(X,Y) \
	if (X == NULL) { \
		if ((X = init_qlic_string()) == NULL) { \
			QLIC_PANIC(); \
		} \
	} \
	X->len = strlen(Y); \
	X->string = (char*)malloc(sizeof(char) * (X->len + 2)); \
	void* __LINE__ptr = strncpy(X->string, Y, X->len); \
	if (__LINE__ptr == NULL) { \
		qlic_error("%d in %s failed\n __LINE__ - 2, __FILE__"); \
		QLIC_PANIC(); \
	}

#endif
