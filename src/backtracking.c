#include "sudoku.h"
#include <stdbit.h>

// Return code:
// >1: Solved
// 0: partially solved
// <1: problem broken (either from the beginning or by passes)
int passSolve(Grid grid)
{
    int madeProgress, i;
    // I experimented with using passes in different ways (or not at all) on diabolical.txt
    do {
//// Times are for diabolical.txt

// 2m17.836s
// We want this pass to catch possible malformed passes in DEBUG mode
#ifdef DEBUG
        madeProgress = runAllPasses(grid);
#endif

// 1m42.592s
// madeProgress = runAllPassesSmart(grid);

// 0m27.316s (but can't reproduce time)
#ifndef DEBUG
        madeProgress = runFastPassesSmart(grid);
#endif

        // This would be pure backtracking
        // 3h30m3.1957s (extrapolated after 7’274 solves)
        // madeProgress = isGridValidPass(grid);
    } while (madeProgress > 0);

    // Check whether sudoku has been solved completely
    if (!madeProgress) {
        for (i = 0; i < 81; i++) {
            if (!grid[i].determined) {
                return 0;
            }
        }
        return 1;
    }
    // Control flow reaches this only if madeProgress <0 => invalid sudoku
    return -1;
}

// The chances of guessing correctly are higher when selecting a cell with fewer candidates
int findCellWithLeastCandidates(Grid grid)
{
    // Naively assumes max 9 candidates for readability
    int found[9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 }, i;
    for (i = 0; i < 81; i++) {
        if (!grid[i].determined) {
            // Assumes every undetermined cell has candidates (== is valid), this is okay imo
            found[stdc_count_ones(grid[i].candidates) - 1] = i;
            // Assuming singleCandidateToDeterminedPass() was run on grid, this check is utterly
            // useless
            if (found[0] >= 0) return found[0];
            if (found[1] >= 0) return found[1];
        }
    }
    for (i = 2; i < 9; i++)
        if (found[i] >= 0) return found[i];
    // Let's assume grid isn't solved because only stupid people would run this function on a
    // completed grid
    return 67;
}

// Copies grid a into grid b (from a grid array)
void copyGrid(Grid* grid, int a, int b)
{
    int i;
    for (i = 0; i < 81; i++) {
        grid[b][i] = grid[a][i];
    }
}

// This function expects a grid array, where the first element is the problem itself and the rest is
// for the backtracking. It is expected that the array is long enough (64 elements is always enough
// because that is the maximum possible number of unsolved cells for a problem to be uniquely
// solvable)
//
// Return code: (No <0;>0 so that I can use switch statement...)
// 1: Solved
// 0: partially solved (impossible with backtracking)
// -1: problem broken (either from the beginning or by passes)
int backtrackSolve(Grid* grid)
{
    int gridCounter = 0, status, guessCell, guessNumber;
    while (1) {
        status = passSolve(grid[gridCounter]);
        if (status > 0) {
            // Yay, problem solved
            if (gridCounter) copyGrid(grid, gridCounter, 0);
            // printf("%d\n", gridCounter);
            // fflush(stdout);
            return 1;
        }

        else if (status == 0) {
            // Nay, problem partially solved, need further guessing
            copyGrid(grid, gridCounter, gridCounter + 1);
            guessCell = findCellWithLeastCandidates(grid[gridCounter]);
            guessNumber = stdc_first_trailing_one(grid[gridCounter][guessCell].candidates);
            grid[gridCounter][guessCell].candidates &= ~(1 << (guessNumber - 1));
            grid[gridCounter + 1][guessCell].determined = true;
            grid[gridCounter + 1][guessCell].number = guessNumber;
            gridCounter++;
        }

        else if (status < 0) {
            // Problem (thus past guess) is wrong
            gridCounter--;
            if (gridCounter < 0) {
                // Original input problem is broken
                return -1;
            }
        }

        else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiv-by-zero"
            return 42 / 0;
#pragma GCC diagnostic pop
        }
    }
}
