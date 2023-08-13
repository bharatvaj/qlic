#include <qresponder.h>
#include <stdio.h>
#include <string.h>
#include <qcommon.h>

int qlic_handle_read_chat(char* response, size_t item_size, size_t nmemb, void *userp) {
	debug("helo: %s\n", response);
	size_t bytes = item_size * nmemb;
	debug("Test: item_size %zu", item_size);
	debug("read_message: userp: %s\n", (char*)userp);
	debug("Test: response %s", response);
	return bytes;
}

size_t qlic_handle_send_message(char* response, size_t item_size, size_t nmemb, void *userp) {
	size_t bytes = item_size * nmemb;
	inform("Chunk size: %zu\n", nmemb);
	inform("send_message: userp: %s\n", (char*)userp);
	inform("Response: %s\n", response);
	return bytes;
}
