# qlic version
VERSION=0.2.5

# Customize below to fit your system
CFLAGS=-I. -I${HOME}/sysroot/include -g -Wall -Wextra -DQLIC_VERSION=\"${VERSION}\" -DJSMN_STATIC
LDFLAGS=-L${HOME}/sysroot/lib -loauth -lqstr -lcurl
