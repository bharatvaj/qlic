VERSION = 0.1

# Customize below to fit your system

# Paths
PREFIX=/usr/local
MANPREFIX = $(PREFIX)/$(DESTDIR)/share/man
DESTDIR=


# Unix like
INSTALL=install -m766
RM=rm -rf
MKDIR=mkdir -p
OUT=-o
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Os -I.

# Windows
INSTALL=cp
OUT=/out:
CFLAGS+=-D_CRT_SECURE_NO_WARNINGS
