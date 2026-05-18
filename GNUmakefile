#!/usr/bin/make -f

CC = gcc
CFLAGS = -std=c23 -O3 -march=native -MMD
CFLAGS_DEBUG = -std=c23 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fanalyzer -DDEBUG -MMD
SRC_DIR = src/
OUT_DIR = out/
OBJ_DIR = $(OUT_DIR)obj/
SRC = main backtracking gridPasses gridPrinter

sudokuSolver: $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC)))
	@mkdir -p $(OUT_DIR)
	$(CC) $^ -o $(OUT_DIR)sudokuSolver $(CFLAGS)

sudokuSolverDebug: $(addprefix $(OBJ_DIR), $(addsuffix _debug.o, $(SRC)))
	@mkdir -p $(OUT_DIR)
	$(CC) $^ -o $(OUT_DIR)sudokuSolverDebug $(CFLAGS_DEBUG)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $< -c $(CFLAGS) -o $@

$(OBJ_DIR)%_debug.o: $(SRC_DIR)%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $< -c $(CFLAGS_DEBUG) -o $@

-include $(OBJ_DIR)*.d

clean:
	rm -rf $(OUT_DIR) $(OBJ_DIR)

.PHONY: sudokuSolver sudokuSolverDebug clean
