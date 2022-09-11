SRC = qlic.c qlic_response_handler.c qlic_common.c cliq_apis.c
HDR = qlic_oauth.h

include config.mk

$(OBJ): $(SRC)

all: options $(TARGET)

options:
	@echo qlic build options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

ifneq (CC,cl)
TARGET=qlic.exe
else
TARGET=qlic
endif

$(TARGET): $(OBJ)

.PHONY: all options

all: qlic

test: qlic
	./qlic -r
clean:
	rm qlic
