#include "sudoku.h"

// TODO actually implement backtracking
int backtrackSolve(Grid grid)
{
    int madeProgress, i;
    do {
        if (!(madeProgress = runCheapPasses(grid))) {
            madeProgress = runAllPasses(grid);
        }
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
