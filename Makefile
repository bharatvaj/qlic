qlic: qlic.c qlic_response_handler.* qlic_common.* cliq_apis.* qlic_oauth.h
	$(CC) qlic.c qlic_response_handler.c qlic_common.c cliq_apis.c -o qlic -I. -I${DESTDIR}/include -L${DESTDIR}/lib -lnxjson -loauth -lcurl -g

test: qlic
	./qlic -r
clean:
	rm qlic
