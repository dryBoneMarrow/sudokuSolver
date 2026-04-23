#include <stdbit.h>
#include <stdio.h>

#define GO_RIGHT(pos, offset) (pos = ((pos) + 1) % 9 + 9 * (offset))
#define GO_DOWN(pos) ((pos) = ((pos) + 9) % 81)

#define ESC "\x1B"
#define CSI ESC "["

#define BOLDD CSI "1m%d" CSI "0m"
#define FAINTD CSI "2m%d" CSI "0m"

#define CUR_GO_UP CSI "%dA"
#define CUR_GO_DOWN CSI "%dB"
#define CUR_GO_RIGHT CSI "%dC"
#define CUR_GO_LEFT CSI "%dD"
// Those are private sequences, hopefully they work
#define CUR_SAVE_POSITION CSI "s"
#define CUR_RESTORE_POSITION CSI "u"

typedef struct {
    bool determined;
    union {
        int number;
        unsigned _BitInt(9) candidates;
    };
} Grid[81];

#ifdef DEBUG
char blobbyGlobi(Grid* grid, int i)
{
    if ((*grid)[i].determined) return (*grid)[i].number + '0';
    return ' ';
}

// This isn't prod code => Can be ugly (Vim motions ftw)
void printGrid(Grid* g)
{
    fprintf(stderr,
        "╭───┬───┬───┰───┬───┬───┰───┬───┬───╮\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "├───┼───┼───╂───┼───┼───╂───┼───┼───┤\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "├───┼───┼───╂───┼───┼───╂───┼───┼───┤\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "┝━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┥\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "├───┼───┼───╂───┼───┼───╂───┼───┼───┤\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "├───┼───┼───╂───┼───┼───╂───┼───┼───┤\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "┝━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┥\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "├───┼───┼───╂───┼───┼───╂───┼───┼───┤\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "├───┼───┼───╂───┼───┼───╂───┼───┼───┤\n"
        "│ %c │ %c │ %c ┃ %c │ %c │ %c ┃ %c │ %c │ %c │\n"
        "╰───┴───┴───┸───┴───┴───┸───┴───┴───╯\n",
        // clang-format off
        // to save space
blobbyGlobi(g,0),blobbyGlobi(g,1),blobbyGlobi(g,2),blobbyGlobi(g,3),blobbyGlobi(g,4),blobbyGlobi(g,5),blobbyGlobi(g,6),blobbyGlobi(g,7),blobbyGlobi(g,8),blobbyGlobi(g,9),blobbyGlobi(g,10),blobbyGlobi(g,11),blobbyGlobi(g,12),blobbyGlobi(g,13),blobbyGlobi(g,14),blobbyGlobi(g,15),blobbyGlobi(g,16),blobbyGlobi(g,17),blobbyGlobi(g,18),blobbyGlobi(g,19),blobbyGlobi(g,20),blobbyGlobi(g,21),blobbyGlobi(g,22),blobbyGlobi(g,23),blobbyGlobi(g,24),blobbyGlobi(g,25),blobbyGlobi(g,26),blobbyGlobi(g,27),blobbyGlobi(g,28),blobbyGlobi(g,29),blobbyGlobi(g,30),blobbyGlobi(g,31),blobbyGlobi(g,32),blobbyGlobi(g,33),blobbyGlobi(g,34),blobbyGlobi(g,35),blobbyGlobi(g,36),blobbyGlobi(g,37),blobbyGlobi(g,38),blobbyGlobi(g,39),blobbyGlobi(g,40),blobbyGlobi(g,41),blobbyGlobi(g,42),blobbyGlobi(g,43),blobbyGlobi(g,44),blobbyGlobi(g,45),blobbyGlobi(g,46),blobbyGlobi(g,47),blobbyGlobi(g,48),blobbyGlobi(g,49),blobbyGlobi(g,50),blobbyGlobi(g,51),blobbyGlobi(g,52),blobbyGlobi(g,53),blobbyGlobi(g,54),blobbyGlobi(g,55),blobbyGlobi(g,56),blobbyGlobi(g,57),blobbyGlobi(g,58),blobbyGlobi(g,59),blobbyGlobi(g,60),blobbyGlobi(g,61),blobbyGlobi(g,62),blobbyGlobi(g,63),blobbyGlobi(g,64),blobbyGlobi(g,65),blobbyGlobi(g,66),blobbyGlobi(g,67),blobbyGlobi(g,68),blobbyGlobi(g,69),blobbyGlobi(g,70),blobbyGlobi(g,71),blobbyGlobi(g,72),blobbyGlobi(g,73),blobbyGlobi(g,74),blobbyGlobi(g,75),blobbyGlobi(g,76),blobbyGlobi(g,77),blobbyGlobi(g,78),blobbyGlobi(g,79),blobbyGlobi(g,80));}
// clang-format on

// For the grid to be filled properly the terminal has to be as big (particularly as high) as the
// grid itself (wontfix because debug only)
void printBigGrid(Grid* grid)
{
    int i;

    printf(CUR_SAVE_POSITION);

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

    printf(CUR_SAVE_POSITION);
    for (i = 0; i < 81; i++) {
        printf(CUR_RESTORE_POSITION);
        printf(CUR_GO_UP, 35 - (int)(i / 9) * 4);
        printf(CUR_GO_RIGHT, 4 + (i % 9) * 8);
        if ((*grid)[i].determined)
            printf(BOLDD, (*grid)[i].number);
        else {
            if ((*grid)[i].candidates & 1 << 0)
                printf(CUR_GO_UP CUR_GO_LEFT FAINTD CUR_GO_DOWN CUR_GO_RIGHT, 1, 2, 1, 1, 1);
            if ((*grid)[i].candidates & 1 << 1)
                printf(CUR_GO_UP FAINTD CUR_GO_DOWN CUR_GO_LEFT, 1, 2, 1, 1);
            if ((*grid)[i].candidates & 1 << 2)
                printf(CUR_GO_UP CUR_GO_RIGHT FAINTD CUR_GO_DOWN CUR_GO_LEFT, 1, 2, 3, 1, 3);
            if ((*grid)[i].candidates & 1 << 3) printf(CUR_GO_LEFT FAINTD CUR_GO_RIGHT, 2, 4, 1);
            if ((*grid)[i].candidates & 1 << 4) printf(FAINTD CUR_GO_LEFT, 5, 1);
            if ((*grid)[i].candidates & 1 << 5) printf(CUR_GO_RIGHT FAINTD CUR_GO_LEFT, 2, 6, 3);
            if ((*grid)[i].candidates & 1 << 6)
                printf(CUR_GO_DOWN CUR_GO_LEFT FAINTD CUR_GO_UP CUR_GO_RIGHT, 1, 2, 7, 1, 1);
            if ((*grid)[i].candidates & 1 << 7)
                printf(CUR_GO_DOWN FAINTD CUR_GO_UP CUR_GO_LEFT, 1, 8, 1, 1);
            if ((*grid)[i].candidates & 1 << 8)
                printf(CUR_GO_DOWN CUR_GO_RIGHT FAINTD CUR_GO_UP CUR_GO_LEFT, 1, 2, 9, 1, 3);
        }
    }
    printf(CUR_RESTORE_POSITION);
}

#endif

bool cleanRow(Grid* grid, const int row)
{
    int i, position = 9 * row;
    bool somethingChanged = false;
    //// Collect fixed numbers

    // C23 goated
    unsigned _BitInt(9) fixedNums = 0;
    // (~= bool fixedNum[9] = { };)
    for (i = 0; i < 9; i++) {
        // Naively assumes no conflicts (e.g. no number twice in a row)
        if ((*grid)[position].determined) fixedNums |= 1 << ((*grid)[position].number - 1);
        GO_RIGHT(position, row);
    }

    // Remove all fixed numbers as option from undetermined numbers
    for (i = 0; i < 9; i++) {
        if (!(*grid)[position].determined)
        // False positive warninge, see ISO C23 6.5.17: «An assignment expression has the value of
        // the left operand after the assignment»
        // Or I'm too stupid, this doesn't seem to be the case here:
        // https://stackoverflow.com/questions/10623114/operation-on-may-be-undefined (candidates is
        // only modifed once, isn't it?)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"
            if ((*grid)[position].candidates != ((*grid)[position].candidates &= ~fixedNums))
#pragma GCC diagnostic pop
                somethingChanged = true;
        GO_RIGHT(position, row);
    }
    return somethingChanged;
}

bool cleanColumn(Grid* grid, int column)
{
    int i;
    bool somethingChanged = false;
    //// Collect fixed numbers

    // C23 goated
    unsigned _BitInt(9) fixedNums = 0;
    // (~= bool fixedNum[9] = { };)
    for (i = 0; i < 9; i++) {
        // Naively assumes no conflicts (e.g. no number twice in a column)
        if ((*grid)[column].determined) fixedNums |= 1 << ((*grid)[column].number - 1);
        GO_DOWN(column);
    }

    // Remove all fixed numbers as option from undetermined numbers
    for (i = 0; i < 9; i++) {
        if (!(*grid)[column].determined)
        // False positive warninge, see ISO C23 6.5.17: «An assignment expression has the value of
        // the left operand after the assignment»
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"
            if ((*grid)[column].candidates != ((*grid)[column].candidates &= ~fixedNums))
#pragma GCC diagnostic pop
                somethingChanged = true;
        GO_DOWN(column);
    }
    return somethingChanged;
}

bool cleanSubgrid(Grid* grid, const int subGrid)
{
    // Instead of GO_DOWN or GO_RIGHT macros because would be unnecessarily complex (space time
    // tradeoff);
    static const short nextElementInSubgrid[9][9] = { { 0, 1, 2, 9, 10, 11, 18, 19, 20 },
        { 3, 4, 5, 12, 13, 14, 21, 22, 23 }, { 6, 7, 8, 15, 16, 17, 24, 25, 26 },
        { 27, 28, 29, 36, 37, 38, 45, 46, 47 }, { 30, 31, 32, 39, 40, 41, 48, 49, 50 },
        { 33, 34, 35, 42, 43, 44, 51, 52, 53 }, { 54, 55, 56, 63, 64, 65, 72, 73, 74 },
        { 57, 58, 59, 66, 67, 68, 75, 76, 77 }, { 60, 61, 62, 69, 70, 71, 78, 79, 80 } };
    int i;
    bool somethingChanged = false;

    //// Collect fixed numbers
    // C23 goated
    unsigned _BitInt(9) fixedNums = 0;
    // (~= bool fixedNum[9] = { };)
    for (i = 0; i < 9; i++) {
        // Naively assumes no conflicts (e.g. no number twice in a column)
        if ((*grid)[nextElementInSubgrid[subGrid][i]].determined)
            fixedNums |= 1 << ((*grid)[nextElementInSubgrid[subGrid][i]].number - 1);
    }

    // Remove all fixed numbers as option from undetermined numbers
    for (i = 0; i < 9; i++) {
        if (!(*grid)[nextElementInSubgrid[subGrid][i]].determined)
        // False positive warninge, see ISO C23 6.5.17: «An assignment expression has the value of
        // the left operand after the assignment»
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"
            if ((*grid)[nextElementInSubgrid[subGrid][i]].candidates
                != ((*grid)[nextElementInSubgrid[subGrid][i]].candidates &= ~fixedNums))
#pragma GCC diagnostic pop
                somethingChanged = true;
    }
    return somethingChanged;
}

void solve(Grid* grid)
{
    bool hasChanged;
    int i;
    do {
        hasChanged = false;
        for (i = 0; i < 9; i++) {
            hasChanged |= cleanRow(grid, i);
            hasChanged |= cleanColumn(grid, i);
            hasChanged |= cleanSubgrid(grid, i);
        }
        // [...]
        for (i = 0; i < 81; i++) {
            if (!(*grid)[i].determined && stdc_has_single_bit((*grid)[i].candidates)) {
                (*grid)[i].determined = true;
                (*grid)[i].number = stdc_first_trailing_one((*grid)[i].candidates);
                // #ifdef DEBUG
                // fprintf(stderr, "%dx%d: %d\n", i % 9 + 1, i / 9 + 1, (*grid)[i].number);
                // #endif
            }
        }

        // #ifdef DEBUG
        //         printGrid(grid);
        // #endif
    } while (hasChanged);
}

int main()
{
// #ifdef DEBUG
//     printBigGrid(nullptr);
// #endif
#ifdef DEBUG
    Grid grid;
    FILE* puzzleInput = fopen("easy.txt", "r");
    int i, j, k, currNum;

    for (i = 0; i < 100; i++) {
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

        printBigGrid(&grid);
        solve(&grid);
        printBigGrid(&grid);
        for (k = 0; k < 81; k++) {
            if (!grid[k].determined) goto home;
        }

        fgetc(puzzleInput);
    }
home:

//// Solve
// solve(&grid);
#endif
}
