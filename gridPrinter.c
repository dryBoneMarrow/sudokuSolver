#include <stdio.h>

#include "ANSIEscapeSequences.h"
#include "sudoku.h"

// TODO revive old printer for no ascii (compile option?) and to small terminal output (even
// solution not in a grid for even smaller output?)

void printGrid(Grid grid)
// For the grid to be filled properly the terminal has to be as big (particularly as high) as the
// grid itself (wontfix because debug / testing only)
{
    int i;

    // Storing the whole string in one line would result in a stack overflow (string literal max ist
    // 4095 in C23)
    static char topLine[]
        = "╭───────┬───────┬───────┰───────┬───────┬───────┰───────┬───────┬───────╮\n";
    static char insideField[]
        = "│       │       │       ┃       │       │       ┃       │       │       │\n";
    static char fieldSeperator[]
        = "├───────┼───────┼───────╂───────┼───────┼───────╂───────┼───────┼───────┤\n";
    static char fatFieldSeperator[]
        = "┝━━━━━━━┿━━━━━━━┿━━━━━━━╋━━━━━━━┿━━━━━━━┿━━━━━━━╋━━━━━━━┿━━━━━━━┿━━━━━━━┥\n";
    static char bottomLine[]
        = "╰───────┴───────┴───────┸───────┴───────┴───────┸───────┴───────┴───────╯\n";

    printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", topLine,
        insideField, insideField, insideField, fieldSeperator, insideField, insideField,
        insideField, fieldSeperator, insideField, insideField, insideField, fatFieldSeperator,
        insideField, insideField, insideField, fieldSeperator, insideField, insideField,
        insideField, fieldSeperator, insideField, insideField, insideField, fatFieldSeperator,
        insideField, insideField, insideField, fieldSeperator, insideField, insideField,
        insideField, fieldSeperator, insideField, insideField, insideField, bottomLine);

    // Escape code magic
    printf(CUR_SAVE_POSITION);
    for (i = 0; i < 81; i++) {
        printf(CUR_RESTORE_POSITION);
        printf(CUR_GO_UP, 35 - (int)(i / 9) * 4);
        printf(CUR_GO_RIGHT, 4 + (i % 9) * 8);
        if (grid[i].determined)
            printf(BOLDD, grid[i].number);
        else {
            if (grid[i].candidates & 1 << 0)
                printf(CUR_GO_UP CUR_GO_LEFT FAINTD CUR_GO_DOWN CUR_GO_RIGHT, 1, 2, 1, 1, 1);
            if (grid[i].candidates & 1 << 1)
                printf(CUR_GO_UP FAINTD CUR_GO_DOWN CUR_GO_LEFT, 1, 2, 1, 1);
            if (grid[i].candidates & 1 << 2)
                printf(CUR_GO_UP CUR_GO_RIGHT FAINTD CUR_GO_DOWN CUR_GO_LEFT, 1, 2, 3, 1, 3);
            if (grid[i].candidates & 1 << 3) printf(CUR_GO_LEFT FAINTD CUR_GO_RIGHT, 2, 4, 1);
            if (grid[i].candidates & 1 << 4) printf(FAINTD CUR_GO_LEFT, 5, 1);
            if (grid[i].candidates & 1 << 5) printf(CUR_GO_RIGHT FAINTD CUR_GO_LEFT, 2, 6, 3);
            if (grid[i].candidates & 1 << 6)
                printf(CUR_GO_DOWN CUR_GO_LEFT FAINTD CUR_GO_UP CUR_GO_RIGHT, 1, 2, 7, 1, 1);
            if (grid[i].candidates & 1 << 7)
                printf(CUR_GO_DOWN FAINTD CUR_GO_UP CUR_GO_LEFT, 1, 8, 1, 1);
            if (grid[i].candidates & 1 << 8)
                printf(CUR_GO_DOWN CUR_GO_RIGHT FAINTD CUR_GO_UP CUR_GO_LEFT, 1, 2, 9, 1, 3);
        }
    }
    printf(CUR_RESTORE_POSITION);
}
