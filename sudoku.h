#ifndef _SUDOKU_H
#define _SUDOKU_H

typedef struct {
    bool determined;
    // union {
    short number;
    unsigned _BitInt(9) candidates;
    // };
} Grid[81];

void printGrid(Grid grid);

int runAllPasses(Grid grid);
int runCheapPasses(Grid grid);
#endif
