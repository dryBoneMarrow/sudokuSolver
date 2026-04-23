CC = gcc
CFLAGS = -std=c23 -O3 -march=native
CFLAGS_DEBUG = -std=c23 -g -Wall -Wextra -Wpedantic -fsanitize=address -D DEBUG

all: main.c
	$(CC) *.c -o main $(CFLAGS)

all_debug: main.c
	$(CC) *.c -o main_debug $(CFLAGS_DEBUG)

run: all
	./main

run_debug: all_debug
	./main_debug

clean:
	rm -f main main_debug

.PHONY: all all_debug run run_debug clean
