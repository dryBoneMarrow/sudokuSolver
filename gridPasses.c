#include "sudoku.h"
#include <stdbit.h>

// Macros for readability
#define currRowPos(houseNr, posInHouse) grid[rowToIndex(houseNr, posInHouse)]
#define currColumnPos(houseNr, posInHouse) grid[columnToIndex(houseNr, posInHouse)]
#define currSubgridPos(houseNr, posInHouse) grid[subgridToIndex(houseNr, posInHouse)]

int rowToIndex(int row, int pos)
{
    return pos + 9 * row;
}

int columnToIndex(int column, int pos)
{
    return pos * 9 + column;
}

int subgridToIndex(int subgrid, int pos)
{
    static const short subgridIndices[9][9] = { { 0, 1, 2, 9, 10, 11, 18, 19, 20 },
        { 3, 4, 5, 12, 13, 14, 21, 22, 23 }, { 6, 7, 8, 15, 16, 17, 24, 25, 26 },
        { 27, 28, 29, 36, 37, 38, 45, 46, 47 }, { 30, 31, 32, 39, 40, 41, 48, 49, 50 },
        { 33, 34, 35, 42, 43, 44, 51, 52, 53 }, { 54, 55, 56, 63, 64, 65, 72, 73, 74 },
        { 57, 58, 59, 66, 67, 68, 75, 76, 77 }, { 60, 61, 62, 69, 70, 71, 78, 79, 80 } };

    return subgridIndices[subgrid][pos];
}

enum house { ROW, COLUMN, SUBGRID };

//// Removes obvious non-candidates
// e.g.: if 7 is already in the row, remove it as candidates in undetermined cells of this row
bool simpleCleanPass(Grid grid)
{
    bool somethingChanged = false;

    int i, j;
    for (i = 0; i < 9; i++) {
        // C23, yay
        unsigned _BitInt(9) fixedNums[3] = { };
        // Collect fixed numbers
        for (j = 0; j < 9; j++) {
            if (currRowPos(i, j).determined) fixedNums[ROW] |= 1 << (currRowPos(i, j).number - 1);

            if (currColumnPos(i, j).determined)
                fixedNums[COLUMN] |= 1 << (currColumnPos(i, j).number - 1);

            if (currSubgridPos(i, j).determined)
                fixedNums[SUBGRID] |= 1 << (currSubgridPos(i, j).number - 1);
        }
        // Remove fixed numbers as candidates from undetermined cells
        for (j = 0; j < 9; j++) {
            if (!currRowPos(i, j).determined)
                if (currRowPos(i, j).candidates != (currRowPos(i, j).candidates &= ~fixedNums[ROW]))
                    somethingChanged = true;

            if (!currColumnPos(i, j).determined)
                if (currColumnPos(i, j).candidates
                    != (currColumnPos(i, j).candidates &= ~fixedNums[COLUMN]))
                    somethingChanged = true;

            if (!currSubgridPos(i, j).determined)
                if (currSubgridPos(i, j).candidates
                    != (currSubgridPos(i, j).candidates &= ~fixedNums[SUBGRID]))
                    somethingChanged = true;
        }
    }
    return somethingChanged;
}

// Find candidates that only occur once in a house
bool candidateOnlyInOneCellOfHouse(Grid grid)
{
    bool somethingChanged = false;

    int i, j, k;
    for (i = 0; i < 9; i++) {
        unsigned _BitInt(9) notLonely[3] = { };
        unsigned _BitInt(9) alreadyAppeared[3] = { };
        for (j = 0; j < 9; j++) {
            // If a candidate occurs once, put it in alreadyAppeared, if it occurs more, put it in
            // notLonely
            // Determined numbers go to notLonely directly
            if (currRowPos(i, j).determined) notLonely[ROW] |= 1 << (currRowPos(i, j).number - 1);
            if (!currRowPos(i, j).determined) {
                for (k = 0; k < 9; k++) {
                    if (currRowPos(i, j).candidates & 1 << k) {
                        // candidate found already
                        if (alreadyAppeared[ROW] & 1 << k) notLonely[ROW] |= 1 << k;
                        // First occurence
                        else
                            alreadyAppeared[ROW] |= 1 << k;
                    }
                }
            }

            if (currColumnPos(i, j).determined)
                notLonely[COLUMN] |= 1 << (currColumnPos(i, j).number - 1);
            if (!currColumnPos(i, j).determined) {
                for (k = 0; k < 9; k++) {
                    if (currColumnPos(i, j).candidates & 1 << k) {
                        // candidate found already
                        if (alreadyAppeared[COLUMN] & 1 << k) notLonely[COLUMN] |= 1 << k;
                        // First occurence
                        else
                            alreadyAppeared[COLUMN] |= 1 << k;
                    }
                }
            }

            if (currSubgridPos(i, j).determined)
                notLonely[SUBGRID] |= 1 << (currSubgridPos(i, j).number - 1);
            if (!currSubgridPos(i, j).determined) {
                for (k = 0; k < 9; k++) {
                    if (currSubgridPos(i, j).candidates & 1 << k) {
                        // candidate found already
                        if (alreadyAppeared[SUBGRID] & 1 << k) notLonely[SUBGRID] |= 1 << k;
                        // First occurence
                        else
                            alreadyAppeared[SUBGRID] |= 1 << k;
                    }
                }
            }
        }
        // Every number _not_ int notLonely is lonely and thus has to be the determined number in
        // its cell
        // Attention: We loop through digits of notLonely, not the cells
        for (j = 0; j < 9; j++) {
            // j is a lonely candidate
            if (~notLonely[ROW] & 1 << j) {
                // Loop through cells to find the one holding the lonely candidate
                for (k = 0; k < 9; k++) {
                    if (!currRowPos(i, k).determined && currRowPos(i, k).candidates & 1 << j) {
                        // Could also already make it determined, but why would we have written the
                        // singleCadidateToDeterminedPass then?
                        currRowPos(i, k).candidates = 1 << j;
                        somethingChanged = true;
                        break;
                    }
                }
            }

            if (~notLonely[COLUMN] & 1 << j) {
                // Loop through cells to find the one holding the lonely candidate
                for (k = 0; k < 9; k++) {
                    if (!currColumnPos(i, k).determined
                        && currColumnPos(i, k).candidates & 1 << j) {
                        // Could also already make it determined, but why would we have written the
                        // singleCadidateToDeterminedPass then?
                        currColumnPos(i, k).candidates = 1 << j;
                        somethingChanged = true;
                        break;
                    }
                }
            }

            if (~notLonely[SUBGRID] & 1 << j) {
                // Loop through cells to find the one holding the lonely candidate
                for (k = 0; k < 9; k++) {
                    if (!currSubgridPos(i, k).determined
                        && currSubgridPos(i, k).candidates & 1 << j) {
                        // Could also already make it determined, but why would we have written the
                        // singleCadidateToDeterminedPass then?
                        currSubgridPos(i, k).candidates = 1 << j;
                        somethingChanged = true;
                        break;
                    }
                }
            }
        }
    }
    return somethingChanged;
}

// Changes cells with only one candidates to determined
bool singleCandidateToDeterminedPass(Grid grid)
{
    int i;
    // Hopefully changes were catched earlier, but we never know
    bool somethingChanged = false;
    for (i = 0; i < 81; i++) {
        if (!grid[i].determined && stdc_has_single_bit(grid[i].candidates)) {
            grid[i].determined = true;
            grid[i].number = stdc_first_trailing_one(grid[i].candidates);
            somethingChanged = true;
        }
    }
    return somethingChanged;
}

bool isGridInvalidPass(Grid grid)
{
    int i, j;
    for (i = 0; i < 9; i++) {
        unsigned _BitInt(9) foundNumbers[3] = { };
        for (j = 0; j < 9; j++) {
            if (currRowPos(i, j).determined)
                if (foundNumbers[ROW] == (foundNumbers[ROW] |= 1 << (currRowPos(i, j).number - 1)))
                    return true;

            if (currColumnPos(i, j).determined)
                if (foundNumbers[COLUMN]
                    == (foundNumbers[COLUMN] |= 1 << (currColumnPos(i, j).number - 1)))
                    return true;

            if (currSubgridPos(i, j).determined)
                if (foundNumbers[SUBGRID]
                    == (foundNumbers[SUBGRID] |= 1 << (currSubgridPos(i, j).number - 1)))
                    return true;
        }
    }
    return false;
}

// Runs all passes
// Return codes:
// >0: Problem has been modified
// 0: Problem hasn't been modified (=> no progess)
// <0: Problem is invalid (either invalid from the beginning or pass is malformed)
int runAllPasses(Grid grid)
{
    int madeProgress = false;
    madeProgress |= simpleCleanPass(grid);
    madeProgress |= candidateOnlyInOneCellOfHouse(grid);
    madeProgress |= singleCandidateToDeterminedPass(grid);
    // [...]

    // Special pass with different return code interpretation
    if (isGridInvalidPass(grid)) madeProgress = -1;
    return madeProgress;
}
