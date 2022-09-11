# qlic version
VERSION=0.3.1

# Customize below to fit your system
DESTDIR=${HOME}/sysroot

ifeq (CC,cl)
SHELL=cmd
CFLAGS=/I. /I$(DESTDIR)/include
LDFLAGS=/link /LIBPATH:$(DESTDIR)/lib nxjson.lib oauth.lib curl.lib
else
CFLAGS=-I. -I$(DESTDIR)/include -g -Wall -Wextra -DQLIC_VERSION=\"${VERSION}\" -DJSMN_STATIC
LDFLAGS=-L$(DESTDIR)/lib -loauth -lqstr -lcurl
endif

ifeq (CC,cl)
%.obj : %.c
	$(CC)  /out:$@ $(CFLAGS) $<

%: %.obj
	$(CC) /out:$@ $(LDFLAGS) $<
endif

ifeq (CC,cl)
TARGET=qlic.exe
else
TARGET=qlic
endif

ifeq (CC,cl)
OBJ = $(SRC:.c=.obj)
else
OBJ = $(SRC:.c=.o)
endif
