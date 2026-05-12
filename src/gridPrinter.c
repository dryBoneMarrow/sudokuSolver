#include <stdio.h>
#include <string.h>

#include "ANSIEscapeSequences.h"
#include "sudoku.h"

void printGrid(Grid g, bool compact, FILE* output)
// For the grid to be filled properly the terminal has to be as big (particularly as high) as the
// grid itself (wontfix because debug / testing only)
{
    if (!compact) {
        int i, j, currCell, positionInCell;

        static char topLine[]
            = "╭───────┬───────┬───────┰───────┬───────┬───────┰───────┬───────┬───────╮\n";
        static char insideField[]
            = "│ %s %s %s │ %s %s %s │ %s %s %s ┃ %s %s %s │ %s %s %s │ %s %s %s ┃ %s %s %s │ %s "
              "%s %s │ %s %s %s │\n";
        static char fieldSeperator[]
            = "├───────┼───────┼───────╂───────┼───────┼───────╂───────┼───────┼───────┤\n";
        static char fatFieldSeperator[]
            = "┝━━━━━━━┿━━━━━━━┿━━━━━━━╋━━━━━━━┿━━━━━━━┿━━━━━━━╋━━━━━━━┿━━━━━━━┿━━━━━━━┥\n";
        static char bottomLine[]
            = "╰───────┴───────┴───────┸───────┴───────┴───────┸───────┴───────┴───────╯\n";

        // Contains the formatted content of each cell rtl
        // 10 found by «bruteforce» (thanks address sanitizer)
        char subCellContent[729][10];
        char* cellValue;
        for (i = 0; i < 729; i++) {
            currCell = (i / 81) * 9 + (i % 27 / 3);
            positionInCell = (i % 81 / 27) * 3 + i % 3;
            // undetermied
            if (!g[currCell].determined) {
                if (g[currCell].candidates & 1 << positionInCell) {
                    cellValue = FAINT_START "X" FAINT_END;
                    strcpy(subCellContent[i], cellValue);
                    subCellContent[i][4] = positionInCell + '1';
                } else
                    strcpy(subCellContent[i], " ");
            }
            // determined
            else {
                // is middle of cell
                if (positionInCell == 4) {
                    cellValue = BOLD_START "X" BOLD_END;
                    strcpy(subCellContent[i], cellValue);
                    subCellContent[i][4] = g[currCell].number + '0';
                } else {
                    strcpy(subCellContent[i], " ");
                }
            }
        }

        int currSubCell = 0;
        fputs(topLine, output);
        for (i = 1; i < 18; i++) {
            if (i % 2) {
                for (j = 0; j < 3; j++) {
                    fprintf(output, insideField, subCellContent[currSubCell],
                        subCellContent[currSubCell + 1], subCellContent[currSubCell + 2],
                        subCellContent[currSubCell + 3], subCellContent[currSubCell + 4],
                        subCellContent[currSubCell + 5], subCellContent[currSubCell + 6],
                        subCellContent[currSubCell + 7], subCellContent[currSubCell + 8],
                        subCellContent[currSubCell + 9], subCellContent[currSubCell + 10],
                        subCellContent[currSubCell + 11], subCellContent[currSubCell + 12],
                        subCellContent[currSubCell + 13], subCellContent[currSubCell + 14],
                        subCellContent[currSubCell + 15], subCellContent[currSubCell + 16],
                        subCellContent[currSubCell + 17], subCellContent[currSubCell + 18],
                        subCellContent[currSubCell + 19], subCellContent[currSubCell + 20],
                        subCellContent[currSubCell + 21], subCellContent[currSubCell + 22],
                        subCellContent[currSubCell + 23], subCellContent[currSubCell + 24],
                        subCellContent[currSubCell + 25], subCellContent[currSubCell + 26]);
                    currSubCell += 27;
                }
            } else if (!(i % 6)) {
                fputs(fatFieldSeperator, output);
            } else {
                fputs(fieldSeperator, output);
            }
        }
        fputs(bottomLine, output);

    } else {
        fprintf(output,
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
            g[ 0].determined ? g[ 0].number + '0' : ' ', g[ 1].determined ? g[ 1].number + '0' : ' ',
            g[ 2].determined ? g[ 2].number + '0' : ' ', g[ 3].determined ? g[ 3].number + '0' : ' ',
            g[ 4].determined ? g[ 4].number + '0' : ' ', g[ 5].determined ? g[ 5].number + '0' : ' ',
            g[ 6].determined ? g[ 6].number + '0' : ' ', g[ 7].determined ? g[ 7].number + '0' : ' ',
            g[ 8].determined ? g[ 8].number + '0' : ' ', g[ 9].determined ? g[ 9].number + '0' : ' ',
            g[10].determined ? g[10].number + '0' : ' ', g[11].determined ? g[11].number + '0' : ' ',
            g[12].determined ? g[12].number + '0' : ' ', g[13].determined ? g[13].number + '0' : ' ',
            g[14].determined ? g[14].number + '0' : ' ', g[15].determined ? g[15].number + '0' : ' ',
            g[16].determined ? g[16].number + '0' : ' ', g[17].determined ? g[17].number + '0' : ' ',
            g[18].determined ? g[18].number + '0' : ' ', g[19].determined ? g[19].number + '0' : ' ',
            g[20].determined ? g[20].number + '0' : ' ', g[21].determined ? g[21].number + '0' : ' ',
            g[22].determined ? g[22].number + '0' : ' ', g[23].determined ? g[23].number + '0' : ' ',
            g[24].determined ? g[24].number + '0' : ' ', g[25].determined ? g[25].number + '0' : ' ',
            g[26].determined ? g[26].number + '0' : ' ', g[27].determined ? g[27].number + '0' : ' ',
            g[28].determined ? g[28].number + '0' : ' ', g[29].determined ? g[29].number + '0' : ' ',
            g[30].determined ? g[30].number + '0' : ' ', g[31].determined ? g[31].number + '0' : ' ',
            g[32].determined ? g[32].number + '0' : ' ', g[33].determined ? g[33].number + '0' : ' ',
            g[34].determined ? g[34].number + '0' : ' ', g[35].determined ? g[35].number + '0' : ' ',
            g[36].determined ? g[36].number + '0' : ' ', g[37].determined ? g[37].number + '0' : ' ',
            g[38].determined ? g[38].number + '0' : ' ', g[39].determined ? g[39].number + '0' : ' ',
            g[40].determined ? g[40].number + '0' : ' ', g[41].determined ? g[41].number + '0' : ' ',
            g[42].determined ? g[42].number + '0' : ' ', g[43].determined ? g[43].number + '0' : ' ',
            g[44].determined ? g[44].number + '0' : ' ', g[45].determined ? g[45].number + '0' : ' ',
            g[46].determined ? g[46].number + '0' : ' ', g[47].determined ? g[47].number + '0' : ' ',
            g[48].determined ? g[48].number + '0' : ' ', g[49].determined ? g[49].number + '0' : ' ',
            g[50].determined ? g[50].number + '0' : ' ', g[51].determined ? g[51].number + '0' : ' ',
            g[52].determined ? g[52].number + '0' : ' ', g[53].determined ? g[53].number + '0' : ' ',
            g[54].determined ? g[54].number + '0' : ' ', g[55].determined ? g[55].number + '0' : ' ',
            g[56].determined ? g[56].number + '0' : ' ', g[57].determined ? g[57].number + '0' : ' ',
            g[58].determined ? g[58].number + '0' : ' ', g[59].determined ? g[59].number + '0' : ' ',
            g[60].determined ? g[60].number + '0' : ' ', g[61].determined ? g[61].number + '0' : ' ',
            g[62].determined ? g[62].number + '0' : ' ', g[63].determined ? g[63].number + '0' : ' ',
            g[64].determined ? g[64].number + '0' : ' ', g[65].determined ? g[65].number + '0' : ' ',
            g[66].determined ? g[66].number + '0' : ' ', g[67].determined ? g[67].number + '0' : ' ',
            g[68].determined ? g[68].number + '0' : ' ', g[69].determined ? g[69].number + '0' : ' ',
            g[70].determined ? g[70].number + '0' : ' ', g[71].determined ? g[71].number + '0' : ' ',
            g[72].determined ? g[72].number + '0' : ' ', g[73].determined ? g[73].number + '0' : ' ',
            g[74].determined ? g[74].number + '0' : ' ', g[75].determined ? g[75].number + '0' : ' ',
            g[76].determined ? g[76].number + '0' : ' ', g[77].determined ? g[77].number + '0' : ' ',
            g[78].determined ? g[78].number + '0' : ' ', g[79].determined ? g[79].number + '0' : ' ',
            g[80].determined ? g[80].number + '0' : ' ');
        // clang-format on
    }
    fflush(output);
}
