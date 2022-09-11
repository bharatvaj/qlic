include config.mk

all: options libqstr.a

options:
	@echo libqstr build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

test.obj: test.c libqstr.a
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $< -c

test.exe: test.obj
	$(CC) -o $@ $< /link libqstr.a

qstr.c.o: qstr.c qstr.h
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $< -c

libqstr.a: qstr.c.o
	$(AR) cr $@ $<

clean:
	$(RM) libqstr.a *.obj *.o test libqstr-${VERSION}.tar.gz ut

dist: clean
	$(MKDIR) libqstr-${VERSION}
	cp LICENSE Makefile README config.mk qstr.h qstr.c \
		libqstr-${VERSION}
	tar -cf libqstr-${VERSION}.tar libqstr-${VERSION}
	gzip libqstr-${VERSION}.tar
	$(RM) libqstr-${VERSION}

install: libqstr.a
	$(INSTALL) $< $(PREFIX)/$(DESTDIR)/lib
	$(INSTALL) qstr.h $(PREFIX)/$(DESTDIR)/include

uninstall:
	$(RM) $(PREFIX)/$(DESTDIR)/lib/libqstr.a
	$(RM) $(PREFIX)/$(DESTDIR)/include/qstr.h

.PHONY: all options
