#include <stdio.h>

#include "sudoku.h"

int main()
{
    Grid grid, originalCopy;
    // FILE* puzzleInput = fopen("easy.txt", "r");
    FILE* puzzleInput = fopen("medium.txt", "r");
    int i, j, currNum;
    int madeProgress = false;

    // TODO backtrack approach (e.g. array of grids)

    // Read first 100 problems
    for (i = 0; true; i++) {
        // Parse problem
        for (j = 0; j < 81; j++) {
            currNum = getc(puzzleInput) - '0';
            if (currNum) {
                grid[j].determined = true;
                grid[j].number = currNum;
            } else {
                grid[j].determined = false;
                grid[j].candidates = ~0;
            }
        }

        // Copy grid before solving for debugging
        for (j = 0; j < 81; j++) {
            originalCopy[j].determined = grid[j].determined;
            originalCopy[j].number = grid[j].number;
            originalCopy[j].candidates = 0;
        }

        // Solve problem
        do {
            madeProgress = runAllPasses(grid);
        } while (madeProgress > 0);

        // Exit early if problem is invalid
        if (madeProgress < 0) {
            printf("Problem Nr. %d got into a broken state\n", i + 1);
            printGrid(originalCopy);
            goto home;
        }

        // Exit early if problem isn't solved
        for (j = 0; j < 81; j++) {
            if (!grid[j].determined) {
                printf("Couldn't solve Nr. %d\n", i + 1);
                printGrid(originalCopy);
                goto home;
                // break;
            }
        }

        // Newline
        fgetc(puzzleInput);
        if ((currNum = fgetc(puzzleInput)) == EOF) break;
        // ungetc instead of fseek because stdin (NAME?) will work
        else
            ungetc(currNum, puzzleInput);
    }
home:
    printGrid(grid);
}
