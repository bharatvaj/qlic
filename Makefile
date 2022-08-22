include config.mk

SRC = qresponder.c qcommon.c capis.c 
HDR = ${SRC:.c=.h}
OBJ = ${SRC:.c=.o}

%.o: %.c
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<

qlic: config.h $(OBJ)
	$(CC) -o $@ $(CFLAGS)  qlic.c $(OBJ) $(LDFLAGS)

config.h: config.def.h
	cp config.def.h config.h

test: qlic
	./qlic -r

clean:
	rm -fv qlic config.h *.o
