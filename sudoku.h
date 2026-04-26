#ifndef _SUDOKU_H
#define _SUDOKU_H

#include <stdio.h>

typedef struct {
    bool determined;
    // union {
    short number;
    unsigned _BitInt(9) candidates;
    // };
} Grid[81];

void printGrid(Grid grid, bool compact, FILE* output);

int backtrackSolve(Grid* grid);

int runAllPasses(Grid grid);
int runCheapPasses(Grid grid);
#endif
