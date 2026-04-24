#include <stdbit.h>

#include "sudoku.h"

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

// A naked pair is when two cells of a house _only_ have the same two candidates
// Approach in nakedTripletPass is more elegant imo, but I'll keep this because it is completely
// human made
bool nakedPairsPass(Grid grid)
{
    bool somethingChanged = false;
    int i, j, k, l;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            // If potential naked pair is found
            if (!currRowPos(i, j).determined && stdc_count_ones(currRowPos(i, j).candidates) == 2) {
                for (k = 1; k < 9; k++) {
                    if (!currRowPos(i, (j + k) % 9).determined
                        && currRowPos(i, (j + k) % 9).candidates == currRowPos(i, j).candidates) {
                        // Hurray, we found a naked pair
                        for (l = 0; l < 9; l++) {
                            if (l == j || l == (j + k) % 9) continue;
                            // Again, why non whining about sequence points??
                            if (!currRowPos(i, l).determined) {
                                if (currRowPos(i, l).candidates
                                    != (currRowPos(i, l).candidates
                                        &= ~currRowPos(i, j).candidates)) {
                                    somethingChanged = true;
                                }
                            }
                        }
                    }
                }
            }

            if (!currColumnPos(i, j).determined
                && stdc_count_ones(currColumnPos(i, j).candidates) == 2) {
                for (k = 1; k < 9; k++) {
                    if (!currColumnPos(i, (j + k) % 9).determined
                        && currColumnPos(i, (j + k) % 9).candidates
                            == currColumnPos(i, j).candidates) {
                        // Hurray, we found a naked pair
                        for (l = 0; l < 9; l++) {
                            if (l == j || l == (j + k) % 9) continue;
                            if (!currColumnPos(i, l).determined) {
                                if (currColumnPos(i, l).candidates
                                    != (currColumnPos(i, l).candidates
                                        &= ~currColumnPos(i, j).candidates)) {
                                    somethingChanged = true;
                                }
                            }
                        }
                    }
                }
            }

            if (!currSubgridPos(i, j).determined
                && stdc_count_ones(currSubgridPos(i, j).candidates) == 2) {
                for (k = 1; k < 9; k++) {
                    if (!currSubgridPos(i, (j + k) % 9).determined
                        && currSubgridPos(i, (j + k) % 9).candidates
                            == currSubgridPos(i, j).candidates) {
                        // Hurray, we found a naked pair
                        for (l = 0; l < 9; l++) {
                            if (l == j || l == (j + k) % 9) continue;
                            if (!currSubgridPos(i, l).determined) {
                                if (currSubgridPos(i, l).candidates
                                    != (currSubgridPos(i, l).candidates
                                        &= ~currSubgridPos(i, j).candidates)) {
                                    somethingChanged = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return somethingChanged;
}

// Same as naked pair but for three (thus possible that a cell only contains a subset of this)
// Disclaimer: rough outline / approach from Claude AI (contrary to other passes)
bool nakedTripletsPass(Grid grid)
{
    bool somethingChanged = false;
    int i, j, k, l, m;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            if (currRowPos(i, j).determined || stdc_count_ones(currRowPos(i, j).candidates) > 3)
                continue;
            for (k = j + 1; k < 9; k++) {
                if (currRowPos(i, k).determined || stdc_count_ones(currRowPos(i, k).candidates) > 3)
                    continue;
                for (l = k + 1; l < 9; l++) {
                    if (currRowPos(i, l).determined
                        || stdc_count_ones(currRowPos(i, l).candidates) > 3)
                        continue;
                    unsigned _BitInt(9) candidate = currRowPos(i, j).candidates
                        | currRowPos(i, k).candidates | currRowPos(i, l).candidates;
                    if (stdc_count_ones(candidate) == 3) {
                        // => Found triplet
                        for (m = 0; m < 9; m++) {
                            if (m == j || m == k || m == l || currRowPos(i, m).determined) continue;
                            if (currRowPos(i, m).candidates
                                != (currRowPos(i, m).candidates &= ~candidate))
                                somethingChanged = true;
                        }
                    }
                }
            }
        }

        for (j = 0; j < 9; j++) {
            if (currColumnPos(i, j).determined
                || stdc_count_ones(currColumnPos(i, j).candidates) > 3)
                continue;
            for (k = j + 1; k < 9; k++) {
                if (currColumnPos(i, k).determined
                    || stdc_count_ones(currColumnPos(i, k).candidates) > 3)
                    continue;
                for (l = k + 1; l < 9; l++) {
                    if (currColumnPos(i, l).determined
                        || stdc_count_ones(currColumnPos(i, l).candidates) > 3)
                        continue;
                    unsigned _BitInt(9) candidate = currColumnPos(i, j).candidates
                        | currColumnPos(i, k).candidates | currColumnPos(i, l).candidates;
                    if (stdc_count_ones(candidate) == 3) {
                        // => Found triplet
                        for (m = 0; m < 9; m++) {
                            if (m == j || m == k || m == l || currColumnPos(i, m).determined)
                                continue;
                            if (currColumnPos(i, m).candidates
                                != (currColumnPos(i, m).candidates &= ~candidate))
                                somethingChanged = true;
                        }
                    }
                }
            }
        }

        for (j = 0; j < 9; j++) {
            if (currSubgridPos(i, j).determined
                || stdc_count_ones(currSubgridPos(i, j).candidates) > 3)
                continue;
            for (k = j + 1; k < 9; k++) {
                if (currSubgridPos(i, k).determined
                    || stdc_count_ones(currSubgridPos(i, k).candidates) > 3)
                    continue;
                for (l = k + 1; l < 9; l++) {
                    if (currSubgridPos(i, l).determined
                        || stdc_count_ones(currSubgridPos(i, l).candidates) > 3)
                        continue;
                    unsigned _BitInt(9) candidate = currSubgridPos(i, j).candidates
                        | currSubgridPos(i, k).candidates | currSubgridPos(i, l).candidates;
                    if (stdc_count_ones(candidate) == 3) {
                        // => Found triplet
                        for (m = 0; m < 9; m++) {
                            if (m == j || m == k || m == l || currSubgridPos(i, m).determined)
                                continue;
                            if (currSubgridPos(i, m).candidates
                                != (currSubgridPos(i, m).candidates &= ~candidate))
                                somethingChanged = true;
                        }
                    }
                }
            }
        }
    }
    return somethingChanged;
}

// Same as naked triplet but for four (thus possible that a cell only contains a subset of this)
// Disclaimer: rough outline / approach from Claude AI (contrary to other passes)
bool nakedQuadsPass(Grid grid)
{
    bool somethingChanged = false;
    int i, j, k, l, m, n;
    for (i = 0; i < 9; i++) {
        // Row
        for (j = 0; j < 9; j++) {
            if (currRowPos(i, j).determined || stdc_count_ones(currRowPos(i, j).candidates) > 4)
                continue;
            for (k = j + 1; k < 9; k++) {
                if (currRowPos(i, k).determined || stdc_count_ones(currRowPos(i, k).candidates) > 4)
                    continue;
                for (l = k + 1; l < 9; l++) {
                    if (currRowPos(i, l).determined
                        || stdc_count_ones(currRowPos(i, l).candidates) > 4)
                        continue;
                    for (m = l + 1; m < 9; m++) {
                        if (currRowPos(i, m).determined
                            || stdc_count_ones(currRowPos(i, m).candidates) > 4)
                            continue;
                        unsigned _BitInt(9) candidate = currRowPos(i, j).candidates
                            | currRowPos(i, k).candidates | currRowPos(i, l).candidates
                            | currRowPos(i, m).candidates;
                        if (stdc_count_ones(candidate) == 4) {
                            // => Found quad
                            for (n = 0; n < 9; n++) {
                                if (n == j || n == k || n == l || n == m
                                    || currRowPos(i, n).determined)
                                    continue;
                                if (currRowPos(i, n).candidates
                                    != (currRowPos(i, n).candidates &= ~candidate))
                                    somethingChanged = true;
                            }
                        }
                    }
                }
            }
        }

        // Column
        for (j = 0; j < 9; j++) {
            if (currColumnPos(i, j).determined
                || stdc_count_ones(currColumnPos(i, j).candidates) > 4)
                continue;
            for (k = j + 1; k < 9; k++) {
                if (currColumnPos(i, k).determined
                    || stdc_count_ones(currColumnPos(i, k).candidates) > 4)
                    continue;
                for (l = k + 1; l < 9; l++) {
                    if (currColumnPos(i, l).determined
                        || stdc_count_ones(currColumnPos(i, l).candidates) > 4)
                        continue;
                    for (m = l + 1; m < 9; m++) {
                        if (currColumnPos(i, m).determined
                            || stdc_count_ones(currColumnPos(i, m).candidates) > 4)
                            continue;
                        unsigned _BitInt(9) candidate = currColumnPos(i, j).candidates
                            | currColumnPos(i, k).candidates | currColumnPos(i, l).candidates
                            | currColumnPos(i, m).candidates;
                        if (stdc_count_ones(candidate) == 4) {
                            // => Found quad
                            for (n = 0; n < 9; n++) {
                                if (n == j || n == k || n == l || n == m
                                    || currColumnPos(i, n).determined)
                                    continue;
                                if (currColumnPos(i, n).candidates
                                    != (currColumnPos(i, n).candidates &= ~candidate))
                                    somethingChanged = true;
                            }
                        }
                    }
                }
            }
        }

        // Subgrid
        for (j = 0; j < 9; j++) {
            if (currSubgridPos(i, j).determined
                || stdc_count_ones(currSubgridPos(i, j).candidates) > 4)
                continue;
            for (k = j + 1; k < 9; k++) {
                if (currSubgridPos(i, k).determined
                    || stdc_count_ones(currSubgridPos(i, k).candidates) > 4)
                    continue;
                for (l = k + 1; l < 9; l++) {
                    if (currSubgridPos(i, l).determined
                        || stdc_count_ones(currSubgridPos(i, l).candidates) > 4)
                        continue;
                    for (m = l + 1; m < 9; m++) {
                        if (currSubgridPos(i, m).determined
                            || stdc_count_ones(currSubgridPos(i, m).candidates) > 4)
                            continue;
                        unsigned _BitInt(9) candidate = currSubgridPos(i, j).candidates
                            | currSubgridPos(i, k).candidates | currSubgridPos(i, l).candidates
                            | currSubgridPos(i, m).candidates;
                        if (stdc_count_ones(candidate) == 4) {
                            // => Found quad
                            for (n = 0; n < 9; n++) {
                                if (n == j || n == k || n == l || n == m
                                    || currSubgridPos(i, n).determined)
                                    continue;
                                if (currSubgridPos(i, n).candidates
                                    != (currSubgridPos(i, n).candidates &= ~candidate))
                                    somethingChanged = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return somethingChanged;
}

// A hidden pair is when two candidates are only in two cells, but those cells also contain more
// candidates
// https://www.sudokuwiki.org/Hidden_Candidates
bool hiddenPairsPass(Grid grid)
{
    bool somethingChanged = false;
    int i, j, k, l;
    // i is the current house
    for (i = 0; i < 9; i++) {
        // (j,k) is a possible hidden pair
        for (j = 1; j <= 9; j++) {
            for (k = j + 1; k <= 9; k++) {
                // Stores where the pair (j,k) has been found
                unsigned _BitInt(9) pairLocation[3] = { };

                // Row
                for (l = 0; l < 9; l++) {
                    // Pair isn't possible if one of it's numbers is already fixed
                    if (currRowPos(i, l).determined
                        && (currRowPos(i, l).number == j || currRowPos(i, l).number == k))
                        goto skipPairRow;

                    // Skip determined cells
                    if (currRowPos(i, l).determined) continue;

                    // Pair isn't possible if only one number is present
                    if ((currRowPos(i, l).candidates & (1 << (j - 1))
                            && !(currRowPos(i, l).candidates & (1 << (k - 1))))
                        || (currRowPos(i, l).candidates & (1 << (k - 1))
                            && !(currRowPos(i, l).candidates & (1 << (j - 1)))))
                        goto skipPairRow;

                    // Check if pair is present in cell
                    if ((currRowPos(i, l).candidates & (1 << (j - 1)) + (1 << (k - 1)))
                        == (1 << (j - 1)) + (1 << (k - 1)))
                        pairLocation[ROW] |= 1 << l;
                }
                // A valid pair is found
                if (stdc_count_ones(pairLocation[ROW]) == 2) {
                    for (l = 0; l < 9; l++) {
                        // Skip cell if it doesn't contain hidden pair
                        if (!((1 << l) & pairLocation[ROW])) continue;
                        if (currRowPos(i, l).candidates
                            != (currRowPos(i, l).candidates = (1 << (j - 1)) + (1 << (k - 1))))
                            somethingChanged = true;
                    }
                }
            skipPairRow:

                // Column
                for (l = 0; l < 9; l++) {
                    // Pair isn't possible if one of it's numbers is already fixed
                    if (currColumnPos(i, l).determined
                        && (currColumnPos(i, l).number == j || currColumnPos(i, l).number == k))
                        goto skipPairColumn;

                    // Skip determined cells
                    if (currColumnPos(i, l).determined) continue;

                    // Pair isn't possible if only one number is present
                    if ((currColumnPos(i, l).candidates & (1 << (j - 1))
                            && !(currColumnPos(i, l).candidates & (1 << (k - 1))))
                        || (currColumnPos(i, l).candidates & (1 << (k - 1))
                            && !(currColumnPos(i, l).candidates & (1 << (j - 1)))))
                        goto skipPairColumn;

                    // Check if pair is present in cell
                    if ((currColumnPos(i, l).candidates & (1 << (j - 1)) + (1 << (k - 1)))
                        == (1 << (j - 1)) + (1 << (k - 1)))
                        pairLocation[COLUMN] |= 1 << l;
                }
                // A valid pair is found
                if (stdc_count_ones(pairLocation[COLUMN]) == 2) {
                    for (l = 0; l < 9; l++) {
                        // Skip cell if it doesn't contain hidden pair
                        if (!((1 << l) & pairLocation[COLUMN])) continue;
                        if (currColumnPos(i, l).candidates
                            != (currColumnPos(i, l).candidates = (1 << (j - 1)) + (1 << (k - 1))))
                            somethingChanged = true;
                    }
                }
            skipPairColumn:

                // Subgrid
                for (l = 0; l < 9; l++) {
                    // Pair isn't possible if one of it's numbers is already fixed
                    if (currSubgridPos(i, l).determined
                        && (currSubgridPos(i, l).number == j || currSubgridPos(i, l).number == k))
                        goto skipPairSubgrid;

                    // Skip determined cells
                    if (currSubgridPos(i, l).determined) continue;

                    // Pair isn't possible if only one number is present
                    if ((currSubgridPos(i, l).candidates & (1 << (j - 1))
                            && !(currSubgridPos(i, l).candidates & (1 << (k - 1))))
                        || (currSubgridPos(i, l).candidates & (1 << (k - 1))
                            && !(currSubgridPos(i, l).candidates & (1 << (j - 1)))))
                        goto skipPairSubgrid;

                    // Check if pair is present in cell
                    if ((currSubgridPos(i, l).candidates & (1 << (j - 1)) + (1 << (k - 1)))
                        == (1 << (j - 1)) + (1 << (k - 1)))
                        pairLocation[SUBGRID] |= 1 << l;
                }
                // A valid pair is found
                if (stdc_count_ones(pairLocation[SUBGRID]) == 2) {
                    for (l = 0; l < 9; l++) {
                        // Skip cell if it doesn't contain hidden pair
                        if (!((1 << l) & pairLocation[SUBGRID])) continue;
                        if (currSubgridPos(i, l).candidates
                            != (currSubgridPos(i, l).candidates = (1 << (j - 1)) + (1 << (k - 1))))
                            somethingChanged = true;
                    }
                }
            skipPairSubgrid:
            }
        }
    }
    return somethingChanged;
}

// If a certain candidate is only in a {column,row} inside a subgrid, remove it from the whole rest
// of the {column,row}
// https://www.sudokuwiki.org/Intersection_Removal
bool intersectionRemovalPointingPairsOrTriplesPass(Grid grid)
{
    bool somethingChanged = false;
    int i, j, k, l;
    for (i = 0; i < 9; i++) {
        // j is the current row / column inside the subgrid
        for (j = 0; j < 3; j++) {
            // subHouse refers to a part of a column / row that is in a particular subgrid
            unsigned _BitInt(9) possiblyOnlyInSubHouse[2] = { };
            // Collect all candidates in current sub house
            // k is the current cell in the current sub house
            for (k = 0; k < 3; k++) {
                // Row
                if (!currSubgridPos(i, k + 3 * j).determined) {
                    possiblyOnlyInSubHouse[ROW] |= currSubgridPos(i, k + 3 * j).candidates;
                }

                // Column
                if (!currSubgridPos(i, 3 * k + j).determined) {
                    possiblyOnlyInSubHouse[COLUMN] |= currSubgridPos(i, 3 * k + j).candidates;
                }
            }
            // Remove possible candidates that are outside of sub house
            for (k = 1; k < 3; k++) {
                for (l = 0; l < 3; l++) {
                    // Row
                    if (!currSubgridPos(i, 3 * ((k + j) % 3) + l).determined) {
                        possiblyOnlyInSubHouse[ROW]
                            &= ~currSubgridPos(i, 3 * ((k + j) % 3) + l).candidates;
                    }

                    // Column
                    if (!currSubgridPos(i, (k + j) % 3 + l * 3).determined) {
                        possiblyOnlyInSubHouse[COLUMN]
                            &= ~currSubgridPos(i, (k + j) % 3 + l * 3).candidates;
                    }
                }
            }

            // Now we have all candidates that are indeed only in the subhouse, we can remove those
            // from every othe cell in the row / column

            if (possiblyOnlyInSubHouse[ROW]) {
                int rowNumber = i / 3 * 3 + j;
                for (k = 0; k < 9; k++) {
                    if (i % 3 * 3 <= k && k < i % 3 * 3 + 3) continue;
                    if (!currRowPos(rowNumber, k).determined) {
                        // ?? Wieso hier kein compiler geheule wegen sequence points ??
                        if (currRowPos(rowNumber, k).candidates
                            != (currRowPos(rowNumber, k).candidates
                                &= ~possiblyOnlyInSubHouse[ROW]))
                            somethingChanged = true;
                    }
                }
            }

            if (possiblyOnlyInSubHouse[COLUMN]) {
                int columnNumber = i % 3 * 3 + j;
                for (k = 0; k < 9; k++) {
                    if (i / 3 * 3 <= k && k < i / 3 * 3 + 3) continue;
                    if (!currColumnPos(columnNumber, k).determined) {
                        if (currColumnPos(columnNumber, k).candidates
                            != (currColumnPos(columnNumber, k).candidates
                                &= ~possiblyOnlyInSubHouse[COLUMN]))
                            somethingChanged = true;
                    }
                }
            }
        }
    }
    return somethingChanged;
}

bool intersectionRemovalBoxLineReductionPass(Grid grid)
{
    bool somethingChanged = false;
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
            // Invalid if number occurs twice in house
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

            // Invalid if cell has no candidates left
            if (!grid[i * 9 + j].determined && !grid[i * 9 + j].candidates) return true;
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
    madeProgress |= nakedPairsPass(grid);
    madeProgress |= nakedTripletsPass(grid);
    madeProgress |= nakedQuadsPass(grid);
    madeProgress |= hiddenPairsPass(grid);
    madeProgress |= intersectionRemovalPointingPairsOrTriplesPass(grid);
    madeProgress |= intersectionRemovalBoxLineReductionPass(grid);
    madeProgress |= singleCandidateToDeterminedPass(grid);
    // TODO at least twins and triplets
    // Also intersection removal type 2
    //
    // - Naked pair -> done
    // - naked triplet -> done
    // - naked quad -> done
    // (quints and higher always imply quad or lower thus unnecessary)
    //
    // - hidden pair -> done
    // - hidden triplet -> wip
    // - hidden quad
    //
    // - intersecion box line reduction -> wip
    // [...]

    // Special pass with different return code interpretation
    if (isGridInvalidPass(grid)) madeProgress = -1;
    return madeProgress;
}
