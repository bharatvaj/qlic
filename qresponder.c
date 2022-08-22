#include <qresponder.h>
#include <stdio.h>
#include <string.h>

int qlic_handle_read_chat(char* response, size_t item_size, size_t nmemb, void *userp) {
	printf("helo: %s\n", response);
	size_t bytes = item_size * nmemb;
#define QLIC_TEST_TEST "{\"text\": \"Hi\"}"
	printf("Test: item_size %zu", item_size);
	printf("read_message: userp: %s\n", (char*)userp);
	printf("Test: response %s", response);
	return bytes;
}

size_t qlic_handle_send_message(char* response, size_t item_size, size_t nmemb, void *userp) {
	size_t bytes = item_size * nmemb;
	printf("Chunk size: %zu\n", nmemb);
	printf("send_message: userp: %s\n", (char*)userp);
	printf("%s\n", response);
	return bytes;
}
