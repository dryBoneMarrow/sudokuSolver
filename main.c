#include <stdio.h>

#include "sudoku.h"

// Expects a stream with 81 digits, moves fp
void parseProblem(FILE* input, Grid grid)
{
    // Parse problem
    int j, currNum;
    for (j = 0; j < 81; j++) {
        currNum = getc(input) - '0';
        if (currNum) {
            grid[j].determined = true;
            grid[j].number = currNum;
        } else {
            grid[j].determined = false;
            grid[j].candidates = ~0;
        }
    }
}

int main()
{
    Grid grid, gridCopy;
    // FILE* puzzleInput = fopen("easy.txt", "r");
    // FILE* puzzleInput = fopen("medium.txt", "r");
    FILE* puzzleInput = fopen("hard.txt", "r");
    // FILE* puzzleInput = fopen("diabolical.txt", "r");
    int i, j, newlineOrEOF, unsolvedCounter = 0;
    int isSolved;
    bool firstUnsolved = true;

    // Read problems
    for (i = 0; true; i++) {
        parseProblem(puzzleInput, grid);

        // Copy grid before solving
        for (j = 0; j < 81; j++) {
            gridCopy[j].determined = grid[j].determined;
            gridCopy[j].number = grid[j].number;
            gridCopy[j].candidates = 0;
        }

        // Solve problem
        isSolved = backtrackSolve(grid);

        // Exit early if problem is invalid (hopefully problem was invalid from the beginning)
        if (isSolved < 0) {
            printGrid(gridCopy);
            printGrid(grid);
            printf("Problem Nr. %d got into a broken state\n", i + 1);
            goto exit;
        }

        // Check whether problem has been solved
        if (!isSolved) {
            if (firstUnsolved) {
                printGrid(gridCopy);
                printGrid(grid);
                printf("Couldn't solve Nr. %d\n", i + 1);
                firstUnsolved = false;
                fflush(stdout);
            }
            unsolvedCounter++;
        }

        // Newline
        fgetc(puzzleInput);
        if ((newlineOrEOF = fgetc(puzzleInput)) == EOF) break;
        // ungetc instead of fseek because stdin will work
        else
            ungetc(newlineOrEOF, puzzleInput);
    }
    // Correct counter for statistics
    i++;
    printf(
        "%d/%d (%.2f%%) solved\n", i - unsolvedCounter, i, (double)(i - unsolvedCounter) / i * 100);
exit:
}
