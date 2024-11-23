TEST_BIN = ./tests/test_suite
LIB = libbitarr.a

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic

ifdef FAST
	CFLAGS += -DNDEBUG
endif

LIB_SRC = bit_array.c
LIB_OBJ = $(LIB_SRC:.c=.o)
TEST_SRC = ./tests/*.c
TEST_OBJ = $(TEST_SRC:.c=.o)

.SILENT:

all: $(LIB)
	rm -f *.o

$(LIB): $(LIB_OBJ)
	ar rcs $@ $<

$(LIB_OBJ): $(LIB_SRC)
	$(CC) $(CFLAGS) -O3 -c $<

$(TEST_BIN):
	$(CC) $(CFLAGS) -w -o $(TEST_BIN) $(TEST_SRC)

# Build without error checking
fast:
	$(MAKE) FAST=1 all -s

clean:
	rm -f $(LIB) $(LIB_OBJ)

tests: $(TEST_BIN)
	./$(TEST_BIN)
	rm -f $(TEST_BIN) $(TEST_OBJ)

.PHONY: all clean tests fast
