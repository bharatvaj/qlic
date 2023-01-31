# qlic version
VERSION=0.3.1

# Customize below to fit your system
DESTDIR=${HOME}/sysroot

ifeq (CC,cl)
SHELL=cmd
CFLAGS=/I. /I$(DESTDIR)/include
LDFLAGS=/link /LIBPATH:$(DESTDIR)/lib oauth.lib curl.lib
else
CFLAGS=-I. -I$(DESTDIR)/include -Iout/include -g -Wall -Wextra -DQLIC_VERSION=\"${VERSION}\" -DJSMN_STATIC
LDFLAGS=-Loauth2/ -loauth2 -Lqstr/ -lqstr -L$(DESTDIR) -lcurl
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
