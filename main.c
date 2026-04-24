#include <stdio.h>

#include "sudoku.h"

int main()
{
    Grid grid, originalCopy;
    // FILE* puzzleInput = fopen("easy.txt", "r");
    // FILE* puzzleInput = fopen("medium.txt", "r");
    FILE* puzzleInput = fopen("hard.txt", "r");
    // FILE* puzzleInput = fopen("diabolical.txt", "r");
    int i, j, currNum, unsolvedCounter = 0;
    int madeProgress = false;
    bool firstUnsolved = true;

    // TODO backtrack approach (e.g. array of grids)

    // Read problems
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

        // Copy grid before solving
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
            printGrid(originalCopy);
            printGrid(grid);
            printf("Problem Nr. %d got into a broken state\n", i + 1);
            goto exit;
        }

        // Check whether problem has been solved
        for (j = 0; j < 81; j++) {
            if (!grid[j].determined) {
                if (firstUnsolved) {
                    printGrid(originalCopy);
                    printGrid(grid);
                    printf("Couldn't solve Nr. %d\n", i + 1);
                    firstUnsolved = false;
                    fflush(stdout);
                }
                unsolvedCounter++;
                break;
            }
        }

        // Newline
        fgetc(puzzleInput);
        if ((currNum = fgetc(puzzleInput)) == EOF) break;
        // ungetc instead of fseek because stdin will work
        else
            ungetc(currNum, puzzleInput);
    }
    // Correct counter for statistics
    i++;
    printf(
        "%d/%d (%.2f%%) solved\n", i - unsolvedCounter, i, (double)(i - unsolvedCounter) / i * 100);
exit:
}
