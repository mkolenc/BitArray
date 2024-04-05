BIN = bit_array_test_suite
CC = gcc
CFLAGS = -std=c11 -w

SRC = bit_array_test_suite.c

.SILENT:

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN)
tests: $(BIN)
	./$(BIN)
	$(MAKE) clean -s

.PHONY: all clean tests