qlic: qlic.c
	$(CC) qlic.c qlic_response_handler.c qlic_common.c cliq_apis.c -o qlic -I. -lcurl -g

test: qlic
	./qlic -r
clean:
	rm qlic
