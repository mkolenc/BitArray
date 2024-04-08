TEST_BIN = ./tests/bit_array_test_suite
TEST_SRC = ./tests/bit_array_test_suite.c
LIB = libbitarr.a

CC = gcc
CFLAGS = -std=c11 -w -O3

ifdef FAST
	CFLAGS += -DBIT_ARRAY_FAST
endif

LIB_SRC = bit_array.c
LIB_OBJ = $(LIB_SRC:.c=.o)
TEST_SRC = ./tests/bit_array_test_suite.c
TEST_OBJ = $(TEST_SRC:.c=.o)

.SILENT:

all: $(LIB)
	rm -f *.o

$(LIB): $(LIB_OBJ)
	ar rcs $@ $<

$(LIB_OBJ): $(LIB_SRC)
	$(CC) $(CFLAGS) -c $<

$(TEST_BIN):
	$(CC) $(CFLAGS) -o $(TEST_BIN) $(TEST_SRC)

# Build without error checking
fast:
	$(MAKE) FAST=1 all -s

clean:
	rm -f $(LIB) *.o

tests: $(TEST_BIN)
	./$(TEST_BIN)
	rm -f $(TEST_BIN)

.PHONY: all clean tests fast
