#ifndef _SUDOKU_H
#define _SUDOKU_H

#include <stdio.h>

typedef struct cell {
    bool determined;
    // Union not possible because of singleCandidateToDeterminedPass (reads both members in one
    // assignment; "If rhs and lhs overlap in memory (e.g. they are members of the same union), the
    // behavior is undefined unless the overlap is exact and the types are compatible.": Overlap
    // starts at same place but short isn't 9 bits
    // (https://en.cppreference.com/c/language/operator_assignment))

    // union {
    short number;
    unsigned _BitInt(9) candidates;
    // };
} Grid[81];

void printGrid(Grid grid, bool compact, FILE* output);

int backtrackSolve(Grid* grid);

// int runAllPasses(Grid grid);
// int runAllPassesSmart(Grid grid);
int runFastPassesSmart(Grid grid);
int isGridValidPass(Grid grid);
#endif
