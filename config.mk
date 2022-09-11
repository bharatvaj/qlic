SHELL=cmd
DESTDIR=C:/Users/Administrator/what2/sysroot-qlic/
ifneq (CC,cl)
CFLAGS=/I C:/Users/Administrator/what/qlic /I $(DESTDIR)/include
LDFLAGS=/link /LIBPATH:$(DESTDIR)/lib nxjson.lib oauth.lib curl.lib
else
CFLAGS=-I${PWD} -I$(DESTDIR)
LDFLAGS=-L$(DESTDIR) -lnxjson -loauth -lcurl -g
endif

ifneq (CC,cl)
%.obj : %.c
	$(CC)  /out:$@ $(CFLAGS) $<

%: %.obj
	$(CC) /out:$@ $(LDFLAGS) $<
endif


ifneq (CC,cl)
OBJ = $(SRC:.c=.obj)
else
OBJ = $(SRC:.c=.o)
endif

