/* See LICENSE file for copyright and license details. */

#include <qstr.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef QSTR_ALLOCATOR
#include <stdlib.h>
#define QSTR_ALLOCATOR &malloc
#endif

#ifndef QSTR_REALLOCATOR
#include <stdlib.h>
#define QSTR_REALLOCATOR &realloc
#endif

#ifndef QSTR_MEMCPY
#include <string.h>
#define QSTR_MEMCPY &memcpy
#endif

#ifndef QSTR_DEALLOC
#include <stdlib.h>
#define QSTR_DEALLOC &free
#endif

static qstr_alloc __qstr_allocator = QSTR_ALLOCATOR;
static qstr_realloc __qstr_reallocator = QSTR_REALLOCATOR;
static qstr_memcpy __qstr_memcpy = QSTR_MEMCPY;
static qstr_dealloc __qstr_dealloc = QSTR_DEALLOC;

/**
 * We get qsthdr when we minus `str` with sizeof(qsthdr)
 * as the memory is continguous.
 */
static inline qstrhdr* qstrtoqstrhdr(const qstr str) {
	return (qstrhdr*) (str - sizeof(qstrhdr));
}

/**
 * Contructs a new qstrhdr with size `n`.
 * TODO check if '*' is necessary when using inline funcs,
 * to prevent copies
 */
static inline qstrhdr* qstrnewalloc(size_t* n) {
	return (qstrhdr*)__qstr_allocator(
		sizeof(qstrhdr)         // size of the header
		+ (sizeof(qstr) * (*n)) // actual size of the string
		+ 1                     // space for the null character
		);
}

qstr qstrmalloc(size_t n) {
	qstrhdr* hdr = qstrnewalloc(&n);
	*hdr = n;
	return (qstr)(hdr + 1);
}

qstr qstrnew(const char* str, size_t n) {
	qstrhdr* hdr = qstrnewalloc(&n);
	*hdr = n;
	char* destptr = (qstr)(hdr + 1);
	memcpy(destptr, str, n + 1);
	return destptr;
}

qstr qstrrealloc(const qstr str, size_t newsize) {
	qstrhdr* hdr = qstrtoqstrhdr(str);
	hdr = (qstrhdr*)__qstr_reallocator(hdr, sizeof(qstrhdr) + newsize + 1);
	*hdr = newsize;
	return (char*)(hdr + 1);
}

void qstrfree(qstr str) {
	qstrhdr* hdr = qstrtoqstrhdr(str);
	__qstr_dealloc(hdr);
}

qstr qstrdup(const qstr oldstr) {
	qstrhdr* oldhdr = qstrtoqstrhdr(oldstr);
	size_t newsize = sizeof(qstrhdr) + *oldhdr + 1;
	qstrhdr* newhdr = __qstr_allocator(newsize);
	__qstr_memcpy(newhdr, oldhdr, newsize);
	return (qstr)(newhdr + 1);
}

int qstrcpy(qstr dest, const qstr src) {
	size_t destlen = qstrlen(dest);
	size_t srclen = qstrlen(src);
	if (destlen >= srclen) {
		memcpy(dest, src, srclen);
		return srclen;
	} else {
		memcpy(dest, src, destlen);
		return destlen;
	}
}

size_t qstrchr(const qstr cstr, int c, char** ptr) {
	size_t slen = qstrlen(cstr);
	size_t i = 0;
	for (; i < slen; i++) {
		if(cstr[i] == c) {
			if (ptr != NULL) {
				*ptr = cstr + i;
			}
			return i;
		}
	}
	return 0;
}

size_t qstrrchr(const qstr cstr, int c, char** ptr) {
	size_t slen = qstrlen(cstr);
	size_t i = slen;
	do {
		if(cstr[i] == c) {
			if (ptr != NULL) {
				*ptr = cstr + i;
			}
			return i;
		}
		i--;
	} while (i > 0);
	// TODO handle this part gracefully
	if (i == 1) {
		i--;
		if(cstr[i] == c) {
			if (ptr != NULL) {
				*ptr = cstr + i;
			}
			return i;
		}
	}
	return -1;
}

int qstrcmp(const qstr str1, const qstr str2) {
	size_t i = 0;
	size_t sz1 = qstrlen(str1);
	size_t sz2 = qstrlen(str2);
	if (sz1 != sz2) {
		return 0;
	}
	for(;(i < sz1) && (str1[i] == str2[i]); i++);
	int res = i == sz1;
	return res? 0 : res;
}

qstr qstrcat(const qstr str1, const qstr str2) {
	size_t sz1 = qstrlen(str1);
	size_t sz2 = qstrlen(str2);
	size_t sz3 = sz1 + sz2;
	qstr cqstr = qstrmalloc(sz3);
	qstrhdr* hdr3 = qstrtoqstrhdr(cqstr);
	*hdr3 = sz3;
	__qstr_memcpy(cqstr, str1, sz1);
	__qstr_memcpy(cqstr + sz1, str2, sz2);
	return cqstr;
}

qstr qstrsprintf(const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	// TODO check if this works everywhere
	int n = vsnprintf(NULL, 0, format, ap);
	qstr ptr = qstrmalloc(n);
	va_end(ap);
	va_start(ap, format);
	// int m =
	vsnprintf(ptr, n + 1, format, ap);
	va_end(ap);
	// TODO check if m == n
	// TODO check the validity of ptr
	// TODO add error handling mechanism
	return ptr;
}
