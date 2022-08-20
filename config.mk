VERSION = 0.1

# Customize below to fit your system

# Paths
PREFIX=/usr/local
MANPREFIX = $(PREFIX)/$(DESTDIR)/share/man
DESTDIR=

CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Os -I.

# Unix like
INSTALL=install -m766
RM=rm -rf
MKDIR=mkdir -p

# Windows
#INSTALL=cp
