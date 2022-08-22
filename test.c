#include <qstr.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#define QSTR_TEST_STRING "Hello"

const char* teststr = QSTR_TEST_STRING;
qstr large = NULL;

int enable_debug = 0;

#define debug(...) \
	if (enable_debug) { \
		fprintf(stderr, __VA_ARGS__); \
	}

int sprintf_test() {
	qstr newstr = NULL;
#define QSTR_SAMPLE_STR "This library is awesome"
	size_t n = sizeof(QSTR_SAMPLE_STR) - 1;
	int m = qstrsprintf(&newstr, "This library is %s", "awesome");
	debug("Expected: %s\n", QSTR_SAMPLE_STR);
	debug("Actual: %s\n", newstr);
	debug("Expected Size: %ld\n", n);
	debug("Actual Size: %ld\n", qstrlen(newstr));
	debug("Return value: %d\n", m);
	return (n == qstrlen(newstr));
}

int sprintf_self_test() {
	qstr newstr = qstrnew("mac is UNIX");
#define QSTR_SELF_SAMPLE_STR "GNU is not UNIX but mac is UNIX"
	size_t n = sizeof(QSTR_SELF_SAMPLE_STR) - 1;
	int m = qstrsprintf(&newstr, "GNU is not UNIX but %s", newstr);
	debug("Expected: %s\n", QSTR_SELF_SAMPLE_STR);
	debug("Actual: %s\n", newstr);
	debug("Expected Size: %ld\n", n);
	debug("Actual Size: %ld\n", qstrlen(newstr));
	debug("Return value: %d\n", m);
	return (n == qstrlen(newstr));
}

qstr grow_buffer_example() {
	const size_t buf_size = 64;
	FILE* fp = fopen("test.c", "r");
	debug("buf_size: %ld\n", buf_size);
	qstr buffer = qstrmalloc(buf_size);
	size_t total_bytes_read = 0;
	size_t cbr = 0;
	while((cbr = fread(buffer + total_bytes_read, 1, buf_size, fp)) > 0) {
		if (cbr > 0) {
			total_bytes_read += cbr;
			size_t newsize = total_bytes_read + buf_size;
			debug("buffer: %p, newsize: %ld, ", buffer, newsize);
			buffer = qstrrealloc(buffer, newsize);
			debug("buffer after qstrrealloc: %p\n", buffer);
		} else {
			// cbr is -1 or 0 when error occurs or during eof
			break;
		}
	}
	fclose(fp);
	return buffer;
}

int find_test(int forward) {
	size_t index = 0;
	int searchchar = 'p';
	char* ptr = NULL;
	if (forward) {
		debug("Searching forward '%c' in 'large'\n", searchchar);
		index = qstrchr(large, searchchar, &ptr);
	} else {
		debug("Searching reverse '%c' in 'large'\n", searchchar);
		index = qstrrchr(large, searchchar, &ptr);
	}
	if (ptr == NULL) {
		debug("Search failed\n");
		return -1;
	}
	debug("Value: %s, Index: %ld\n", ptr, index);
	return index;
}

int strlen_test() {
	size_t teststr_len = strlen(teststr);
	qstr sstr = qstrnnew(teststr, teststr_len);
	qstr copystr = qstrdup(sstr);
	// strlen comparisons
	// All should return the same values as strlen(char*)
	size_t constant_len = strlen(QSTR_TEST_STRING);
	size_t sstr_len = qstrlen(sstr);
	size_t copy_len = strlen(copystr);
	debug("strlen(char*): %ld\n", teststr_len);
	debug("strlen(CONSTANT): %ld\n", constant_len);
	debug("qstrlen(qstrdup(sstr)): %ld\n", sstr_len);
	debug("strlen(qstrdup(sstr)): %ld\n", copy_len);
	return (teststr_len == constant_len) == (sstr_len == copy_len);
}

int grow_buffer_test() {
	large = grow_buffer_example();
	// TODO write better test case
	debug("%s\n", large);
	return large != NULL;
}

#define RUN_TEST(I, F, S) \
	int t##I = F; \
	fprintf(stderr, "%d: %s: %s\n", I, S, t##I? "PASSED" : "FAILED");

int all_test() {
	RUN_TEST(1, strlen_test(), "len")
	RUN_TEST(2, grow_buffer_test(), "grow")
	RUN_TEST(3, find_test(1), "find")
	RUN_TEST(4, find_test(0), "findrev")
	RUN_TEST(5, sprintf_test(), "sprintf")
	RUN_TEST(6, sprintf_self_test(), "sprintf_self")
	return t1 && t2 && t3 && t4 && t5 && t6;
}


int main(int argc, char* argv[]) {
	int res = 1;
    if (argc == 1) {
		// run all tests
		res = all_test();
	} else {
		for(int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "debug") == 0) {
				enable_debug = 1;
			} else if(strcmp(argv[i], "all") == 0) {
				res = all_test();
			} else if(strcmp(argv[i], "length") == 0) {
				res = res && strlen_test();
			} else if (strcmp(argv[i], "grow") == 0) {
				res = res && grow_buffer_test();
			} else if (strcmp(argv[i], "sprintf") == 0) {
				res = res && sprintf_test();
			} else if (strcmp(argv[i], "sprintf_self") == 0) {
				res = res && sprintf_self_test();
			} else if (strcmp(argv[i], "find") == 0) {
				if (large == NULL) {
					large = grow_buffer_example();
				}
				// search forward
				res = res && find_test(1)
				// search backward
				&& find_test(0);
			}
		}
	}
	if (large != NULL) {
		qstrfree(large);
	}
	return res;
}

