#include <stdbit.h>

#include "sudoku.h"

///
/// Every basic solving strategy from sudokuwiki.org has been implemented completely
///
/// Efficiency was considered during implementation, still the algorithms aren't trimmed to be as
/// fast as humanly possible
///

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

// TODO most passes were written before they could report sudoku invalidity, performance would be
// better if we'd catch the earlier (thus already in those passes) for backtracking
//
// Spoiler of future me: Doesn't matter at all, if performance is important passes aren't used
// anyways

//// Removes obvious non-candidates
// e.g.: if 7 is already in the row, remove it as candidates in undetermined cells of this row
int simpleCleanPass(Grid grid)
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
int candidateOnlyInOneCellOfHousePass(Grid grid)
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
                // for (k = 0; k < 9; k++) {
                //     if (currRowPos(i, j).candidates & 1 << k) {
                //         // candidate found already
                //         if (alreadyAppeared[ROW] & 1 << k) notLonely[ROW] |= 1 << k;
                //         // First occurence
                //         else
                //             alreadyAppeared[ROW] |= 1 << k;
                //     }
                // }
                notLonely[ROW] |= alreadyAppeared[ROW] & currRowPos(i, j).candidates;
                alreadyAppeared[ROW] |= currRowPos(i, j).candidates;
            }

            if (currColumnPos(i, j).determined)
                notLonely[COLUMN] |= 1 << (currColumnPos(i, j).number - 1);
            if (!currColumnPos(i, j).determined) {
                // for (k = 0; k < 9; k++) {
                //     if (currColumnPos(i, j).candidates & 1 << k) {
                //         // candidate found already
                //         if (alreadyAppeared[COLUMN] & 1 << k) notLonely[COLUMN] |= 1 << k;
                //         // First occurence
                //         else
                //             alreadyAppeared[COLUMN] |= 1 << k;
                //     }
                // }
                notLonely[COLUMN] |= alreadyAppeared[COLUMN] & currColumnPos(i, j).candidates;
                alreadyAppeared[COLUMN] |= currColumnPos(i, j).candidates;
            }

            if (currSubgridPos(i, j).determined)
                notLonely[SUBGRID] |= 1 << (currSubgridPos(i, j).number - 1);
            if (!currSubgridPos(i, j).determined) {
                // for (k = 0; k < 9; k++) {
                //     if (currSubgridPos(i, j).candidates & 1 << k) {
                //         // candidate found already
                //         if (alreadyAppeared[SUBGRID] & 1 << k) notLonely[SUBGRID] |= 1 << k;
                //         // First occurence
                //         else
                //             alreadyAppeared[SUBGRID] |= 1 << k;
                //     }
                // }
                notLonely[SUBGRID] |= alreadyAppeared[SUBGRID] & currSubgridPos(i, j).candidates;
                alreadyAppeared[SUBGRID] |= currSubgridPos(i, j).candidates;
            }
        }
        // Every number _not_ int notLonely is lonely and thus has to be the determined number in
        // its cell
        // Attention: We loop through digits of notLonely, not the cells
        // TODO could probably be optimized
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
int nakedPairsPass(Grid grid)
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
int nakedTripletsPass(Grid grid)
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
int nakedQuadsPass(Grid grid)
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
int hiddenPairsPass(Grid grid)
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
                        goto skipHiddenPairRow;

                    // Skip determined cells
                    if (currRowPos(i, l).determined) continue;

                    // Pair isn't possible if only one number is present
                    if ((currRowPos(i, l).candidates & (1 << (j - 1))
                            && !(currRowPos(i, l).candidates & (1 << (k - 1))))
                        || (currRowPos(i, l).candidates & (1 << (k - 1))
                            && !(currRowPos(i, l).candidates & (1 << (j - 1)))))
                        goto skipHiddenPairRow;

                    // Check if pair is present in cell
                    if ((currRowPos(i, l).candidates & ((1 << (j - 1)) | (1 << (k - 1))))
                        == ((1 << (j - 1)) | (1 << (k - 1))))
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
            skipHiddenPairRow:

                // Column
                for (l = 0; l < 9; l++) {
                    // Pair isn't possible if one of it's numbers is already fixed
                    if (currColumnPos(i, l).determined
                        && (currColumnPos(i, l).number == j || currColumnPos(i, l).number == k))
                        goto skipHiddenPairColumn;

                    // Skip determined cells
                    if (currColumnPos(i, l).determined) continue;

                    // Pair isn't possible if only one number is present
                    if ((currColumnPos(i, l).candidates & (1 << (j - 1))
                            && !(currColumnPos(i, l).candidates & (1 << (k - 1))))
                        || (currColumnPos(i, l).candidates & (1 << (k - 1))
                            && !(currColumnPos(i, l).candidates & (1 << (j - 1)))))
                        goto skipHiddenPairColumn;

                    // Check if pair is present in cell
                    if ((currColumnPos(i, l).candidates & ((1 << (j - 1)) | (1 << (k - 1))))
                        == ((1 << (j - 1)) | (1 << (k - 1))))
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
            skipHiddenPairColumn:

                // Subgrid
                for (l = 0; l < 9; l++) {
                    // Pair isn't possible if one of it's numbers is already fixed
                    if (currSubgridPos(i, l).determined
                        && (currSubgridPos(i, l).number == j || currSubgridPos(i, l).number == k))
                        goto skipHiddenPairSubgrid;

                    // Skip determined cells
                    if (currSubgridPos(i, l).determined) continue;

                    // Pair isn't possible if only one number is present
                    if ((currSubgridPos(i, l).candidates & (1 << (j - 1))
                            && !(currSubgridPos(i, l).candidates & (1 << (k - 1))))
                        || (currSubgridPos(i, l).candidates & (1 << (k - 1))
                            && !(currSubgridPos(i, l).candidates & (1 << (j - 1)))))
                        goto skipHiddenPairSubgrid;

                    // Check if pair is present in cell
                    if ((currSubgridPos(i, l).candidates & ((1 << (j - 1)) | (1 << (k - 1))))
                        == ((1 << (j - 1)) | (1 << (k - 1))))
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
            skipHiddenPairSubgrid:
            }
        }
    }
    return somethingChanged;
}

// see hiddenPair
int hiddenTripletsPass(Grid grid)
{
    bool somethingChanged = false;
    int i, j, k, l, m;
    // We argue for every house...
    for (i = 0; i < 9; i++) {
        // ... why every hidden triple is present or not (the triple being (j,k,l))
        for (j = 1; j <= 9; j++) {
            for (k = j + 1; k <= 9; k++) {
                for (l = k + 1; l <= 9; l++) {
                    unsigned _BitInt(9) subsetOfTriplePresent[3] = { };
                    const unsigned _BitInt(9) currentTriplet
                        = (1 << (j - 1)) | (1 << (k - 1)) | (1 << (l - 1));
                    unsigned _BitInt(9) currentTripletCopy;

                    // Row
                    for (m = 0; m < 9; m++) {
                        // A triple isn't possible if one of it's numbers is already fixed
                        if (currRowPos(i, m).determined
                            && (currRowPos(i, m).number == j || currRowPos(i, m).number == k
                                || currRowPos(i, m).number == l))
                            goto skipHiddenTripletRow;

                        // Don't need to check detemined cells
                        if (currRowPos(i, m).determined) continue;

                        // Count number of cells containing subset of triplet
                        if (currRowPos(i, m).candidates & currentTriplet) {
                            subsetOfTriplePresent[ROW] |= 1 << m;
                        }
                    }
                    // Subset of trplet has to occur in exactly three cells (less means invalid
                    // sudoku, more means not a hidden triplet)
                    if (stdc_count_ones(subsetOfTriplePresent[ROW]) > 3) goto skipHiddenTripletRow;
                    if (stdc_count_ones(subsetOfTriplePresent[ROW]) < 3) return -1;

                    // Check that all digits of triplet at least once occur
                    currentTripletCopy = currentTriplet;
                    for (m = 0; m < 9; m++) {
                        if (!((1 << m) & subsetOfTriplePresent[ROW])) continue;
                        currentTripletCopy &= ~currRowPos(i, m).candidates;
                    }
                    // Some digit of currently checked triplet is in none of the cells
                    if (currentTripletCopy) goto skipHiddenTripletRow;

                    // Delete all other candidates from the cells containing the triplet
                    for (m = 0; m < 9; m++) {
                        if (!((1 << m) & subsetOfTriplePresent[ROW])) continue;
                        if (currRowPos(i, m).candidates
                            != (currRowPos(i, m).candidates &= currentTriplet))
                            somethingChanged = 1;
                    }

                skipHiddenTripletRow:

                    // Column
                    for (m = 0; m < 9; m++) {
                        // A triple isn't possible if one of it's numbers is already fixed
                        if (currColumnPos(i, m).determined
                            && (currColumnPos(i, m).number == j || currColumnPos(i, m).number == k
                                || currColumnPos(i, m).number == l))
                            goto skipHiddenTripletColumn;

                        // Don't need to check detemined cells
                        if (currColumnPos(i, m).determined) continue;

                        // Count number of cells containing subset of triplet
                        if (currColumnPos(i, m).candidates & currentTriplet) {
                            subsetOfTriplePresent[COLUMN] |= 1 << m;
                        }
                    }
                    // Subset of trplet has to occur in exactly three cells (less means invalid
                    // sudoku, more means not a hidden triplet)
                    if (stdc_count_ones(subsetOfTriplePresent[COLUMN]) > 3)
                        goto skipHiddenTripletColumn;
                    if (stdc_count_ones(subsetOfTriplePresent[COLUMN]) < 3) return -1;

                    // Check that all digits of triplet at least once occur
                    currentTripletCopy = currentTriplet;
                    for (m = 0; m < 9; m++) {
                        if (!((1 << m) & subsetOfTriplePresent[COLUMN])) continue;
                        currentTripletCopy &= ~currColumnPos(i, m).candidates;
                    }
                    // Some digit of currently checked triplet is in none of the cells
                    if (currentTripletCopy) goto skipHiddenTripletColumn;

                    // Delete all other candidates from the cells containing the triplet
                    for (m = 0; m < 9; m++) {
                        if (!((1 << m) & subsetOfTriplePresent[COLUMN])) continue;
                        if (currColumnPos(i, m).candidates
                            != (currColumnPos(i, m).candidates &= currentTriplet))
                            somethingChanged = 1;
                    }

                skipHiddenTripletColumn:

                    // Subgrid
                    for (m = 0; m < 9; m++) {
                        // A triple isn't possible if one of it's numbers is already fixed
                        if (currSubgridPos(i, m).determined
                            && (currSubgridPos(i, m).number == j || currSubgridPos(i, m).number == k
                                || currSubgridPos(i, m).number == l))
                            goto skipHiddenTripletSubgrid;

                        // Don't need to check detemined cells
                        if (currSubgridPos(i, m).determined) continue;

                        // Count number of cells containing subset of triplet
                        if (currSubgridPos(i, m).candidates & currentTriplet) {
                            subsetOfTriplePresent[SUBGRID] |= 1 << m;
                        }
                    }
                    // Subset of trplet has to occur in exactly three cells (less means invalid
                    // sudoku, more means not a hidden triplet)
                    if (stdc_count_ones(subsetOfTriplePresent[SUBGRID]) > 3)
                        goto skipHiddenTripletSubgrid;
                    if (stdc_count_ones(subsetOfTriplePresent[SUBGRID]) < 3) return -1;

                    // Check that all digits of triplet at least once occur
                    currentTripletCopy = currentTriplet;
                    for (m = 0; m < 9; m++) {
                        if (!((1 << m) & subsetOfTriplePresent[SUBGRID])) continue;
                        currentTripletCopy &= ~currSubgridPos(i, m).candidates;
                    }
                    // Some digit of currently checked triplet is in none of the cells
                    if (currentTripletCopy) goto skipHiddenTripletSubgrid;

                    // Delete all other candidates from the cells containing the triplet
                    for (m = 0; m < 9; m++) {
                        if (!((1 << m) & subsetOfTriplePresent[SUBGRID])) continue;
                        if (currSubgridPos(i, m).candidates
                            != (currSubgridPos(i, m).candidates &= currentTriplet))
                            somethingChanged = 1;
                    }

                skipHiddenTripletSubgrid:
                }
            }
        }
    }
    return somethingChanged;
}

// see hiddenPair
int hiddenQuadsPass(Grid grid)
{
    bool somethingChanged = false;
    int i, j, k, l, m, n;
    // We argue for every house...
    for (i = 0; i < 9; i++) {
        // ... why every hidden quad is present or not (the quad being (j,k,l,m))
        for (j = 1; j <= 9; j++) {
            for (k = j + 1; k <= 9; k++) {
                for (l = k + 1; l <= 9; l++) {
                    for (m = l + 1; m <= 9; m++) {

                        unsigned _BitInt(9) subsetOfQuadPresent[3] = { };
                        const unsigned _BitInt(9) currentQuad
                            = (1 << (j - 1)) | (1 << (k - 1)) | (1 << (l - 1)) | (1 << (m - 1));
                        unsigned _BitInt(9) currentQuadCopy;

                        // Row
                        for (n = 0; n < 9; n++) {
                            // A quad isn't possible if one of it's numbers is already fixed
                            if (currRowPos(i, n).determined
                                && (currRowPos(i, n).number == j || currRowPos(i, n).number == k
                                    || currRowPos(i, n).number == l
                                    || currRowPos(i, n).number == m))
                                goto skipHiddenQuadRow;

                            // Don't need to check detemined cells
                            if (currRowPos(i, n).determined) continue;

                            // Count number of cells containing subset of quad
                            if (currRowPos(i, n).candidates & currentQuad) {
                                subsetOfQuadPresent[ROW] |= 1 << n;
                            }
                        }
                        // Subset of quad has to occur in exactly four cells (less means invalid
                        // sudoku, more means not a hidden quad)
                        if (stdc_count_ones(subsetOfQuadPresent[ROW]) > 4) goto skipHiddenQuadRow;
                        if (stdc_count_ones(subsetOfQuadPresent[ROW]) < 4) return -1;

                        // Check that all digits of quad at least once occur
                        currentQuadCopy = currentQuad;
                        for (n = 0; n < 9; n++) {
                            if (!((1 << n) & subsetOfQuadPresent[ROW])) continue;
                            currentQuadCopy &= ~currRowPos(i, n).candidates;
                        }
                        // Some digits of currently checked quad are in none of the cells
                        if (currentQuadCopy) goto skipHiddenQuadRow;

                        // Delete all other candidates from the cells containing the quad
                        for (n = 0; n < 9; n++) {
                            if (!((1 << n) & subsetOfQuadPresent[ROW])) continue;
                            if (currRowPos(i, n).candidates
                                != (currRowPos(i, n).candidates &= currentQuad))
                                somethingChanged = 1;
                        }

                    skipHiddenQuadRow:

                        // Column
                        for (n = 0; n < 9; n++) {
                            // A quad isn't possible if one of it's numbers is already fixed
                            if (currColumnPos(i, n).determined
                                && (currColumnPos(i, n).number == j
                                    || currColumnPos(i, n).number == k
                                    || currColumnPos(i, n).number == l
                                    || currColumnPos(i, n).number == m))
                                goto skipHiddenQuadColumn;

                            // Don't need to check detemined cells
                            if (currColumnPos(i, n).determined) continue;

                            // Count number of cells containing subset of quad
                            if (currColumnPos(i, n).candidates & currentQuad) {
                                subsetOfQuadPresent[COLUMN] |= 1 << n;
                            }
                        }
                        // Subset of quad has to occur in exactly four cells (less means invalid
                        // sudoku, more means not a hidden quad)
                        if (stdc_count_ones(subsetOfQuadPresent[COLUMN]) > 4)
                            goto skipHiddenQuadColumn;
                        if (stdc_count_ones(subsetOfQuadPresent[COLUMN]) < 4) return -1;

                        // Check that all digits of quad at least once occur
                        currentQuadCopy = currentQuad;
                        for (n = 0; n < 9; n++) {
                            if (!((1 << n) & subsetOfQuadPresent[COLUMN])) continue;
                            currentQuadCopy &= ~currColumnPos(i, n).candidates;
                        }
                        // Some digits of currently checked quad are in none of the cells
                        if (currentQuadCopy) goto skipHiddenQuadColumn;

                        // Delete all other candidates from the cells containing the quad
                        for (n = 0; n < 9; n++) {
                            if (!((1 << n) & subsetOfQuadPresent[COLUMN])) continue;
                            if (currColumnPos(i, n).candidates
                                != (currColumnPos(i, n).candidates &= currentQuad))
                                somethingChanged = 1;
                        }

                    skipHiddenQuadColumn:

                        // Subgrid
                        for (n = 0; n < 9; n++) {
                            // A quad isn't possible if one of it's numbers is already fixed
                            if (currSubgridPos(i, n).determined
                                && (currSubgridPos(i, n).number == j
                                    || currSubgridPos(i, n).number == k
                                    || currSubgridPos(i, n).number == l
                                    || currSubgridPos(i, n).number == m))
                                goto skipHiddenQuadSubgrid;

                            // Don't need to check detemined cells
                            if (currSubgridPos(i, n).determined) continue;

                            // Count number of cells containing subset of quad
                            if (currSubgridPos(i, n).candidates & currentQuad) {
                                subsetOfQuadPresent[SUBGRID] |= 1 << n;
                            }
                        }
                        // Subset of quad has to occur in exactly four cells (less means invalid
                        // sudoku, more means not a hidden quad)
                        if (stdc_count_ones(subsetOfQuadPresent[SUBGRID]) > 4)
                            goto skipHiddenQuadSubgrid;
                        if (stdc_count_ones(subsetOfQuadPresent[SUBGRID]) < 4) return -1;

                        // Check that all digits of quad at least once occur
                        currentQuadCopy = currentQuad;
                        for (n = 0; n < 9; n++) {
                            if (!((1 << n) & subsetOfQuadPresent[SUBGRID])) continue;
                            currentQuadCopy &= ~currSubgridPos(i, n).candidates;
                        }
                        // Some digits of currently checked quad are in none of the cells
                        if (currentQuadCopy) goto skipHiddenQuadSubgrid;

                        // Delete all other candidates from the cells containing the quad
                        for (n = 0; n < 9; n++) {
                            if (!((1 << n) & subsetOfQuadPresent[SUBGRID])) continue;
                            if (currSubgridPos(i, n).candidates
                                != (currSubgridPos(i, n).candidates &= currentQuad))
                                somethingChanged = 1;
                        }

                    skipHiddenQuadSubgrid:
                    }
                }
            }
        }
    }
    return somethingChanged;
}

// If a certain candidate is only in a {column,row} inside a subgrid, remove it from the whole rest
// of the {column,row}
// https://www.sudokuwiki.org/Intersection_Removal
int intersectionRemovalPointingPairsOrTriplesPass(Grid grid)
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
                        // ?? Wieso hier kein compiler geheule wegen sequence points ?? TODO
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

// https://www.sudokuwiki.org/Intersection_Removal#:~:text=Box%20Line%20Reduction
int intersectionRemovalBoxLineReductionPass(Grid grid)
{
    bool somethingChanged = false;
    int i, j, k;
    // For every line and column
    for (i = 0; i < 9; i++) {
        // This variable holds every candidate that is inside of a subgrid the current line or
        // column
        unsigned _BitInt(9) candidatesInSubgridAndHouse[2][3] = { };
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 3; k++) {
                if (!currRowPos(i, j * 3 + k).determined)
                    candidatesInSubgridAndHouse[ROW][j] |= currRowPos(i, j * 3 + k).candidates;

                if (!currColumnPos(i, j * 3 + k).determined)
                    candidatesInSubgridAndHouse[COLUMN][j]
                        |= currColumnPos(i, j * 3 + k).candidates;
            }
        }

        for (j = 0; j < 3; j++) {
            unsigned _BitInt(9) numbersOnlyInOneSubgrid[2]
                = { candidatesInSubgridAndHouse[ROW][j], candidatesInSubgridAndHouse[COLUMN][j] };
            for (k = 1; k < 3; k++) {
                numbersOnlyInOneSubgrid[ROW] &= ~candidatesInSubgridAndHouse[ROW][(j + k) % 3];
                numbersOnlyInOneSubgrid[COLUMN]
                    &= ~candidatesInSubgridAndHouse[COLUMN][(j + k) % 3];
            }

            // Row
            if (numbersOnlyInOneSubgrid[ROW]) {
                // Remove those numbers as candidates from every other cell in subgrid
                for (k = 0; k < 9; k++) {
                    if (k >= i % 3 * 3 && k <= i % 3 * 3 + 2) continue;
                    if (!currSubgridPos(i / 3 * 3 + j, k).determined) {
                        if (currSubgridPos(i / 3 * 3 + j, k).candidates
                            != (currSubgridPos(i / 3 * 3 + j, k).candidates
                                &= ~numbersOnlyInOneSubgrid[ROW]))
                            somethingChanged = true;
                    }
                }
            }

            // Column
            if (numbersOnlyInOneSubgrid[COLUMN]) {
                // Remove those numbers as candidates from every other cell in subgrid
                for (k = 0; k < 9; k++) {
                    if (k == i % 3 || k == i % 3 + 3 || k == i % 3 + 6) continue;
                    if (!currSubgridPos(j * 3 + i / 3, k).determined) {
                        if (currSubgridPos(j * 3 + i / 3, k).candidates
                            != (currSubgridPos(j * 3 + i / 3, k).candidates
                                &= ~numbersOnlyInOneSubgrid[COLUMN]))
                            somethingChanged = true;
                    }
                }
            }
        }
    }
    return somethingChanged;
}

// Changes cells with only one candidates to determined
int singleCandidateToDeterminedPass(Grid grid)
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

int isGridValidPass(Grid grid)
{
    int i, j;
    for (i = 0; i < 9; i++) {
        unsigned _BitInt(9) foundNumbers[3] = { };
        for (j = 0; j < 9; j++) {
            // Invalid if number occurs twice in house
            if (currRowPos(i, j).determined)
                if (foundNumbers[ROW] == (foundNumbers[ROW] |= 1 << (currRowPos(i, j).number - 1)))
                    return -1;

            if (currColumnPos(i, j).determined)
                if (foundNumbers[COLUMN]
                    == (foundNumbers[COLUMN] |= 1 << (currColumnPos(i, j).number - 1)))
                    return -1;

            if (currSubgridPos(i, j).determined)
                if (foundNumbers[SUBGRID]
                    == (foundNumbers[SUBGRID] |= 1 << (currSubgridPos(i, j).number - 1)))
                    return -1;

            // Invalid if cell has no candidates left
            if (!grid[i * 9 + j].determined && !grid[i * 9 + j].candidates) return -1;
        }
    }
    return 0;
}

// Runs all passes
// Return codes:
// >0: Problem has been modified
// 0: Problem hasn't been modified (=> no progess)
// <0: Problem is invalid (either invalid from the beginning or pass is malformed)
int runAllPasses(Grid grid)
{
    int madeProgress = false;
    // Return code of all passes identical with runAllPasses
    madeProgress |= simpleCleanPass(grid);
    madeProgress |= candidateOnlyInOneCellOfHousePass(grid);
    madeProgress |= nakedPairsPass(grid);
    madeProgress |= nakedTripletsPass(grid);
    madeProgress |= nakedQuadsPass(grid);
    madeProgress |= hiddenPairsPass(grid);
    madeProgress |= hiddenTripletsPass(grid);
    madeProgress |= hiddenQuadsPass(grid);
    madeProgress |= intersectionRemovalPointingPairsOrTriplesPass(grid);
    madeProgress |= intersectionRemovalBoxLineReductionPass(grid);
    madeProgress |= singleCandidateToDeterminedPass(grid);
    madeProgress |= isGridValidPass(grid);
    // TODO
    // [...]

    return madeProgress;
}

// Runs passes in increasing computing intensive order; exits early if problem was modified
// Return codes:
// >0: Problem has been modified
// 0: Problem hasn't been modified (=> no progess)
// <0: Problem is invalid (either invalid from the beginning or pass is malformed)
int runAllPassesSmart(Grid grid)
{
    int madeProgress = false;
    // Return code of all passes identical with runAllPasses
    // Commented number is number of nested loops for resource intensity estimate
    madeProgress |= simpleCleanPass(grid); // 2
    madeProgress |= candidateOnlyInOneCellOfHousePass(grid); // 3
    if (madeProgress) goto exit;
    madeProgress |= intersectionRemovalBoxLineReductionPass(grid); // 3
    if (madeProgress) goto exit;
    madeProgress |= intersectionRemovalPointingPairsOrTriplesPass(grid); // 4
    if (madeProgress) goto exit;
    madeProgress |= nakedPairsPass(grid); // 4
    if (madeProgress) goto exit;
    madeProgress |= hiddenPairsPass(grid); // 4
    if (madeProgress) goto exit;
    madeProgress |= nakedTripletsPass(grid); // 5
    if (madeProgress) goto exit;
    madeProgress |= hiddenTripletsPass(grid); // 5
    if (madeProgress) goto exit;
    madeProgress |= nakedQuadsPass(grid); // 6
    if (madeProgress) goto exit;
    madeProgress |= hiddenQuadsPass(grid); // 6

exit:
    madeProgress |= singleCandidateToDeterminedPass(grid); // 1
    madeProgress |= isGridValidPass(grid);
    return madeProgress;
}

// Runs passes in increasing computing intensive order; exits early if problem was modified; skips
// passes that achieve rarely results and are resource intensive Return codes: >0: Problem has been
// modified 0: Problem hasn't been modified (=> no progess) <0: Problem is invalid (either invalid
// from the beginning or pass is malformed)
int runFastPassesSmart(Grid grid)
{
    int madeProgress = false;
    // Return code of all passes identical with runAllPasses
    // Commented number is number of nested loops for resource intensity estimate
    madeProgress |= simpleCleanPass(grid); // 2
    madeProgress |= candidateOnlyInOneCellOfHousePass(grid); // 3
    if (madeProgress) goto exit;
    madeProgress |= intersectionRemovalBoxLineReductionPass(grid); // 3
    if (madeProgress) goto exit;
    madeProgress |= intersectionRemovalPointingPairsOrTriplesPass(grid); // 4
    if (madeProgress) goto exit;
    madeProgress |= nakedPairsPass(grid); // 4
    if (madeProgress) goto exit;
    madeProgress |= hiddenPairsPass(grid); // 4
    // if (madeProgress) goto exit;
    // madeProgress |= nakedTripletsPass(grid); // 5
    // if (madeProgress) goto exit;
    // madeProgress |= hiddenTripletsPass(grid); // 5
    // if (madeProgress) goto exit;
    // madeProgress |= nakedQuadsPass(grid); // 6
    // if (madeProgress) goto exit;
    // madeProgress |= hiddenQuadsPass(grid); // 6

exit:
    madeProgress |= singleCandidateToDeterminedPass(grid); // 1
    madeProgress |= isGridValidPass(grid);
    return madeProgress;
}
