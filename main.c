#include <stdio.h>
#include <stdlib.h>

#include "sudoku.h"

#define PRINT_USAGE                                                                                \
    fprintf(stderr,                                                                                \
        "Usage: %s [INPUT [OUTPUT]]\n       stdin and stdout are used by default\n\nFile format: " \
        "One or more lines matching the following pattern: [0-9]{81}\\n (0 denotes an unknown)\n", \
        argv[0])

// problem is a string of at least 81 chars containing digits
// Return code: >0 -> Success
//               0 -> Invalid problem string
int parseProblem(const char* problem, Grid grid)
{
    int i, currNum;
    for (i = 0; i < 81; i++) {
        currNum = problem[i] - '0';
        if (currNum < 0 || currNum > 9) {
            return 0;
        }
        if (currNum) {
            grid[i].determined = true;
            grid[i].number = currNum;
        } else {
            grid[i].determined = false;
            grid[i].candidates = ~0;
        }
    }
    return 1;
}

int main(const int argc, char** argv)
{
    FILE *input, *output;
    // No arguments -> stdin as input and stdout as output are used
    if (argc == 1) {
        input = stdin;
        output = stdout;
    }
    // One argument -> argv[1] as input and stdout as output are used
    else if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "Couldn't open input\n\n");
            PRINT_USAGE;
            return EXIT_FAILURE;
        }
        output = stdout;
    }
    // Two arguments -> argv[1] as input and argv[2] as output
    else if (argc == 3) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "Couldn't open input\n\n");
            PRINT_USAGE;
            return EXIT_FAILURE;
        }
        output = fopen(argv[2], "w");
        if (!output) {
            fprintf(stderr, "Couldn't open output\n\n");
            PRINT_USAGE;
            fclose(input);
            return EXIT_FAILURE;
        }
    } else {
        PRINT_USAGE;
        return EXIT_FAILURE;
    }

    // 81 for each digit + \n + \0
    char currProblemString[83];
    // 64 because every uniquely solvable sudoku has at least 17 clues
    Grid grid[64], gridCopy;
    int i;
    int exit_status = EXIT_SUCCESS;

    while (fgets(currProblemString, 83, input) && !feof(input)) {
        if (ferror(input)) {
            fprintf(stderr, "Error while file reading\n");
            exit_status = EXIT_FAILURE;
            goto exit_cleanup;
        }

        if (!parseProblem(currProblemString, grid[0])) {
            fprintf(stderr, "Invalid puzzle format detected, aborting...\nPuzzle string: %s\n\n",
                currProblemString);
            PRINT_USAGE;
            exit_status = EXIT_FAILURE;
            goto exit_cleanup;
        }

        // We assume backtrackSolve correctly verifies sudoku, thus no further checks about
        // correctness are being made (on solved grid)
        switch (backtrackSolve(grid)) {
        case -1:
            parseProblem(currProblemString, gridCopy);
            // isGridValid only returns -1 or 0, thus no need to check / handle >1
            if (!isGridValidPass(gridCopy)) {
                fprintf(
                    stderr, "Contradictions in puzzle detected while solving; a pass broke it\n");
                fprintf(stderr, "Input:\n");
                printGrid(gridCopy, true, stderr);
                fprintf(stderr, "Current (incorrect) state:\n");
                printGrid(grid[0], false, stderr);
            } else {
                fprintf(stderr, "Contradictions in puzzle input detected\n");
                fprintf(stderr, "Input:\n");
                printGrid(gridCopy, true, stderr);
            }
            exit_status = EXIT_FAILURE;
            goto exit_cleanup;

            // Impossible with backtracking
        case 0:
            fprintf(stderr, "Puzzle has been solved incompletely\n");
            fprintf(stderr, "Input:\n");
            parseProblem(currProblemString, gridCopy);
            printGrid(gridCopy, true, stderr);
            fprintf(stderr, "Current (incorrect) state:\n");
            printGrid(grid[0], false, stderr);
            exit_status = EXIT_FAILURE;
            goto exit_cleanup;

        case 1:

            for (i = 0; i < 81; i++) {
                currProblemString[i] = grid[0][i].number + '0';
            }
            currProblemString[81] = '\n';
            currProblemString[82] = '\0';
            fputs(currProblemString, output);
            if (ferror(output)) {
                fprintf(stderr, "Error while writing to file\n");
                exit_status = EXIT_FAILURE;
                goto exit_cleanup;
            }
        }
    }

exit_cleanup:
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
    return exit_status;
}
