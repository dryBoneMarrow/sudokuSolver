# TODO only compile necessary C files
CC = gcc
CFLAGS = -std=c23 -O3 -march=native
CFLAGS_DEBUG = -std=c23 -g -Wall -Wextra -Wpedantic -fsanitize=address -fanalyzer -DDEBUG

all: src/*.c src/*.h
	mkdir -p out
	$(CC) src/*.c -o out/sudokuSolver $(CFLAGS)

all_debug: src/*.c src/*.h
	mkdir -p out
	$(CC) src/*.c -o out/sudokuSolver_debug $(CFLAGS_DEBUG)

clean:
	rm -r out

.PHONY: all all_debug clean
