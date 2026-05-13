# TODO only compile necessary C files
CC = gcc
CFLAGS = -std=c23 -O3 -march=native
CFLAGS_DEBUG = -std=c23 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fanalyzer -DDEBUG
SRC_DIR = src/
OUT_DIR = out/
OBJ_DIR = $(OUT_DIR)obj/

all: $(OBJ_DIR)backtracking.o $(OBJ_DIR)gridPasses.o $(OBJ_DIR)gridPrinter.o $(OBJ_DIR)main.o
	@mkdir -p $(OUT_DIR)
	$(CC) $^ -o $(OUT_DIR)sudokuSolver $(CFLAGS)

debug: $(OBJ_DIR)backtracking_debug.o $(OBJ_DIR)gridPasses_debug.o $(OBJ_DIR)gridPrinter_debug.o $(OBJ_DIR)main_debug.o
	@mkdir -p $(OUT_DIR)
	$(CC) $^ -o $(OUT_DIR)sudokuSolver_debug $(CFLAGS_DEBUG)

$(OBJ_DIR)gridPrinter.o $(OBJ_DIR)gridPrinter_debug.o: $(SRC_DIR)ANSIEscapeSequences.h

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(SRC_DIR)sudoku.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $< -c $(CFLAGS) -o $@

$(OBJ_DIR)%_debug.o: $(SRC_DIR)%.c $(SRC_DIR)sudoku.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $< -c $(CFLAGS_DEBUG) -o $@

	
run: all
	$(OUT_DIR)sudokuSolver

debug_run: debug
	$(OUT_DIR)sudokuSolver_debug

clean:
	rm -rf $(OUT_DIR) $(OBJ_DIR)

.PHONY: all debug run debug_run clean
