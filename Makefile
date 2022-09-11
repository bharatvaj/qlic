include config.mk

SRC = qresponder.c qcommon.c qoauth.c capis.c
HDR = ${SRC:.c=.h}

all: options $(TARGET)

options:
	@echo qlic build options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"


$(TARGET): $(OBJ)

%.o: %.c %.h
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<

qlic: config.h $(OBJ) qlic.c
	$(CC) -o $@ $(CFLAGS)  qlic.c $(OBJ) $(LDFLAGS)

config.h: config.def.h
	cp config.def.h config.h

test: qlic
	./qlic -d -s Hello CT_2243173252451333824_64396901-B1 CT_2243173252451333824_64396901-B1

clean:
	rm -fv qlic config.h *.o

.PHONY: all options

