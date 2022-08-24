# qlic version
VERSION=2

# Customize below to fit your system
CFLAGS=-I. -I${HOME}/sysroot/include -g -Wall -Wextra -Werror -DQLIC_VERSION=\"${VERSION}\"
LDFLAGS=-L${HOME}/sysroot/lib -loauth -lnxjson -lqstr -lcurl
