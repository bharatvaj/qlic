#include <qlic_response_handler.h>
#include <stdio.h>

int qlic_handle_read_chat(char* response, size_t response_size, size_t nmemb, void *userp) {
	/* printf("helo: %s\n", response); */
	return 0;
}

int qlic_handle_send_message(char* response, size_t response_size, size_t nmemb, void *userp) {
	printf("%s\n", response);
	return 0;
}
