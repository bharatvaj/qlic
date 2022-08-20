include config.mk

all: libqstr.a

options:
	@echo libqstr build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

test: test.c libqstr.a
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

qstr.c.o: qstr.c qstr.h
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $< -c

libqstr.a: qstr.c.o
	$(AR) cr $@ $<

clean:
	$(RM) libqstr.a qstr.c.o test libqstr-${VERSION}.tar.gz

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
