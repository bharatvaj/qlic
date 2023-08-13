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
	$(CC) -o $@ $(CFLAGS) -c $<

setup:
	-git clone https://github.com/bharatvaj/oauth2 -b v0.3.1
	DESTDIR=$(PWD)/out make -C oauth2
	-git clone https://github.com/bharatvaj/qstr -b v0.2
	DESTDIR=$(PWD)/out make -C qstr

qlic: config.h $(OBJ) qlic.c
	$(CC) -o $@ $(CFLAGS)  qlic.c $(OBJ) $(LDFLAGS)

config.h: config.def.h
	cp config.def.h config.h

test: qlic
	./qlic -d -s Hello CT_2243173252451333824_64396901-B1

clean:
	rm -fv qlic config.h *.o

.PHONY: all options

