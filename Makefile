CC=gcc
CFLAGS=-g -fPIC -Ideps -lm -Wall -Wextra -pedantic -std=c17
LDFLAGS=-shared -o

BIN=libhashed.so

OBJFILES=$(wildcard src/*.c)
DEPS=$(wildcard deps/*/*.c)

TESTS = $(patsubst %.c, %, $(wildcard t/*.c))

all:
	$(CC) $(CFLAGS) $(DEPS) $(OBJFILES) $(LDFLAGS) $(BIN)

clean:
	rm -f $(TARGET) $(BIN) $(BIN).so main*

test:
	./scripts/test.bash
	$(MAKE) clean

.PHONY: test clean
