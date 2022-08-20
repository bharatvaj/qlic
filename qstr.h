/* See LICENSE file for copyright and license details. */

#ifndef __QSTR_H
#define __QSTR_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * qstr is a simple typealias to char*
 */
typedef char *qstr;

/**
 * qstrhdr holds the metadata of the string
 * It is a simple typedef to size_t
 * The whole layout in memory looks likes this
 * --------------------------
 * |STRSIZE|...C-STRING...|0|
 * --------------------------
 */
typedef size_t qstrhdr;

/**
 * QSTR_ALLOCATOR can be specified at compile time for changing the default
 * which is &malloc. You may also want to change QSTR_REALLOC, QSTR_MEMCPY and
 * QSTR_DEALLOC when changing the default allocator.
 */

typedef void* (*qstr_alloc)(size_t);
typedef void* (*qstr_realloc)(void*, size_t);
typedef void* (*qstr_memcpy)(void* ,const void*, size_t);
typedef void (*qstr_dealloc)(void*);

/**
 * Reserve `n` bytes. Can use to qstrcpy to copy contents safely.
 */
qstr qstrmalloc(size_t strsiz);

/**
 * This creates a new memory of size `n`, where the `str` will be copied into.
 */
qstr qstrnew(const char* str, size_t n);

/**
 * Grows the `str`'s capacity to `newsize` using the function defined in QSTR_REALLOCATOR.
 */
qstr qstrrealloc(const qstr str, size_t newsize);

/**
 * Frees the metadata and the string.
 */
void qstrfree(qstr);

/**
 * Returns the size of the `str` in O(1) time
 */
static inline size_t qstrlen(const qstr str) {
	return *((qstrhdr*) (str - sizeof(qstrhdr)));
}

/**
 * Allocates memory for a qstr with QSTR_ALLOCATOR and
 * copies content from `oldstr` to the newly allocated memory with
 * QSTR_MEMCPY. The copy is returned.
 */
qstr qstrdup(const qstr oldstr);

/**
 * Copies src to dest determined by abs(destlen - srclen).
 * This ensures that this operation is always safe.
 * Returns positive integer when destlen has enough space for contents
 * of src else retuns negative number indicating the missed characters.
 */
int qstrcpy(qstr dest, const qstr src);

/**
 * Searches `needle` in `haystack`. Returns the `index` of the found char.
 * `ptr` is filled to the ptr of the found char.
 * ptr is unmodified if `needle` is not found.
 * WARNING: `ptr` should be passed to check for failure!
 */
size_t qstrchr(const qstr haystack, int needle, char** ptr);

/**
 * Same as qstrchr but start the search from reverse
 */
size_t qstrrchr(const qstr haystack, int needle, char** ptr);

/**
 * Compares `s1` and `s2` and returns an integer
 * that is based on the strcmp(3) function.
 */
int qstrcmp(const qstr s1, const qstr s2);

/**
 * Returns a newly alloated buffer that is the concat of str1 and str2.
 * New buffer is allocated using QSTR_ALLOCATOR.
 */
qstr qstrcat(const qstr str1, const qstr str2);

/**
 * Returns a string `qstr` filled with the given `format`.
 * Internally calls snprintf.
 */
qstr qstrsprintf(const char* format, ...);

#ifdef __cplusplus
}
#endif
#endif
