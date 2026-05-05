CC = gcc
CFLAGS = -std=c23 -O3 -march=native
CFLAGS_DEBUG = -std=c23 -g -Wall -Wextra -Wpedantic -fsanitize=address -fanalyzer

all: *.c *.h
	$(CC) *.c -o main $(CFLAGS)

all_debug: *.c *.h
	$(CC) *.c -o main_debug $(CFLAGS_DEBUG)

clean:
	rm -f main main_debug

.PHONY: all all_debug clean
