#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define ROOM_DIM 50
char MapGrid[ROOM_DIM][ROOM_DIM];
char DoubledMapGrid[ROOM_DIM][ROOM_DIM*2];

void moveRobot(char dir, int *robotRow, int *robotCol);
void moveRobotDoubled(char dir, int *robotRow, int *robotCol);
void moveBoxesHorizontally(int currentRow, int *robotCol, int colDir);
void moveBoxesVertically(int *robotRow, int currentCol, int rowDir);
void getDir(char dir, int *rowDir, int *colDir);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    /* Set the array to 0, just in case */
    memset (MapGrid, 0, sizeof(MapGrid));

    /* Attempt to open the file */
    char *fileName = argv[1];
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    /* Read the input file into the PlotGrid matrix */
    char *line = NULL;
    size_t maxLen = 0;
    int currentRow = 0;
    int lineLen = 0;
    // bool foundRobot = false;
    char *robotPtr = NULL;
    int robotRow, robotCol;
    int doubledRobotRow, doubledRobotCol;
    int firstChar, secondChar;
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1)
    {
        if (line[0] == '#')
        {
            /* Copy map */
            memcpy (&MapGrid[currentRow][0], line, ROOM_DIM);

            for (int i = 0; i < lineLen; i++)
            {
                switch(line[i])
                {
                    case 'O':
                        firstChar = '[';
                        secondChar = ']';
                        break;
                    case '@':
                        firstChar = '@';
                        secondChar = '.';
                        robotRow = currentRow;
                        robotCol = i;
                        doubledRobotRow = currentRow;
                        doubledRobotCol = 2*i;
                        break;
                    default:
                        firstChar = line[i];
                        secondChar = line[i];
                        break;
                }
                DoubledMapGrid[currentRow][2*i] = firstChar;
                DoubledMapGrid[currentRow][2*i+1] = secondChar;
            }

            currentRow++;
        }
        else if (line[0] != '\n')
        {
            /* Moving instructions for robot */
            for (int i = 0; i < lineLen; i++)
            {
                moveRobot(line[i], &robotRow, &robotCol);
                // printf("Moving: %c\n", line[i]);
                moveRobotDoubled(line[i], &doubledRobotRow, &doubledRobotCol);
                // for (int i = 0; i < ROOM_DIM; i++)
                // {
                //     for (int j = 0; j < ROOM_DIM * 2; j++)
                //     {
                //         printf("%c", DoubledMapGrid[i][j]);
                //     }
                //     printf("\n");
                // }
                // printf("\n");
            }
        }
    }

    long total = 0;
    for (int i = 0; i < ROOM_DIM; i++)
    {
        for (int j = 0; j < ROOM_DIM; j++)
        {
            // printf("%c", MapGrid[i][j]);
            if (MapGrid[i][j] == 'O')
            {
                total += (100 * i) + j;
            }
        }
        // printf("\n");
    }
    // printf("\n");

    long totalDoubledGrid = 0;
    for (int i = 0; i < ROOM_DIM; i++)
    {
        for (int j = 0; j < ROOM_DIM * 2; j++)
        {
            // printf("%c", DoubledMapGrid[i][j]);
            if (DoubledMapGrid[i][j] == '[')
            {
                totalDoubledGrid += (100 * i) + j;
            }
        }
        // printf("\n");
    }

    printf("total GPS is %ld, total for doubled grid is %ld\n",
           total, totalDoubledGrid);

}

void moveRobot(char dir, int *robotRow, int *robotCol)
{
    int rowDir = 0;
    int colDir = 0;
    getDir(dir, &rowDir, &colDir);

    int newRow = *robotRow + rowDir;
    int newCol = *robotCol + colDir;

    if (MapGrid[newRow][newCol] == '.')
    {
        /* Move into open space */
        MapGrid[newRow][newCol] = '@';
        MapGrid[*robotRow][*robotCol] = '.';
        *robotRow = newRow;
        *robotCol = newCol;
    }
    else if (MapGrid[newRow][newCol] == 'O')
    {
        /*
         * Attempt to push a box. First, iterate past all the
         * boxes in that direction
         */
        while (MapGrid[newRow][newCol] == 'O')
        {
            newRow += rowDir;
            newCol += colDir;
        }

        /*
         * We hit either empty space or a wall. If we hit a wall,
         * then do nothing. If we hit empty space, move the boxes
         * and the robot in the correct direction.
         */
        if (MapGrid[newRow][newCol] == '.')
        {
            /* Empty space! */
            MapGrid[newRow][newCol] = 'O';

            /*
             * Put the newRow and newCol back at the position the robot
             * will move into and move the robot
             */
            newRow = *robotRow + rowDir;
            newCol = *robotCol + colDir;
            MapGrid[newRow][newCol] = '@';
            MapGrid[*robotRow][*robotCol] = '.';
            *robotRow = newRow;
            *robotCol = newCol;
        }
    }
}

void moveRobotDoubled(char dir, int *robotRow, int *robotCol)
{
    int rowDir = 0;
    int colDir = 0;
    getDir(dir, &rowDir, &colDir);

    int newRow = *robotRow + rowDir;
    int newCol = *robotCol + colDir;

    if (DoubledMapGrid[newRow][newCol] == '.')
    {
        /* Move into open space */
        DoubledMapGrid[newRow][newCol] = '@';
        DoubledMapGrid[*robotRow][*robotCol] = '.';
        *robotRow = newRow;
        *robotCol = newCol;
    }
    else if (DoubledMapGrid[newRow][newCol] == '[' || DoubledMapGrid[newRow][newCol] == ']')
    {
        /* Attempt to push a box. */
        if (rowDir == 0)
        {
            moveBoxesHorizontally(*robotRow, robotCol, colDir);
        }
        else
        {
            moveBoxesVertically(robotRow, *robotCol, rowDir);
        }
    }
}

void moveBoxesHorizontally(int currentRow, int *robotCol, int colDir)
{
    int currentCol = *robotCol + colDir;
    char boxArr[] = {'[', ']'};
    int boxCharIdx;
    while((DoubledMapGrid[currentRow][currentCol] == '[') ||
          (DoubledMapGrid[currentRow][currentCol] == ']'))
    {
        currentCol += 2*colDir;
    }

    if (DoubledMapGrid[currentRow][currentCol] == '.')
    {
        /*
         * We have space to move. Determine what direction to print the boxes
         * well
         */
        if (colDir == -1)
        {
            boxCharIdx = 0;
        }
        else
        {
            boxCharIdx = 1;
        }

        /* Iterate back to the robot, moving boxes as we go */
        while (DoubledMapGrid[currentRow][currentCol] != '@')
        {
            DoubledMapGrid[currentRow][currentCol] = boxArr[boxCharIdx];
            currentCol -= colDir;
            boxCharIdx = (boxCharIdx + 1) % 2;
        }
        DoubledMapGrid[currentRow][currentCol] = '.';

        *robotCol = *robotCol + colDir;
        DoubledMapGrid[currentRow][*robotCol] = '@';
    }
}

void moveBoxesVertically(int *robotRow, int currentCol, int rowDir)
{
    /*
     * impactedBoxSquares[i][j] is true if the box at position i, j will need to
     * move, and false otherwise.
     */
    bool impactedBoxSquares[ROOM_DIM][ROOM_DIM*2] = {0};
    int currentRow = *robotRow + rowDir;
    bool impactedInCurrentRow = true;

    if (DoubledMapGrid[currentRow][currentCol] == '[')
    {
        impactedBoxSquares[currentRow][currentCol] = true;
        impactedBoxSquares[currentRow][currentCol + 1] = true;
    }
    else
    {
        impactedBoxSquares[currentRow][currentCol - 1] = true;
        impactedBoxSquares[currentRow][currentCol] = true;
    }

    int prevRow;
    bool canMoveBoxes = true;
    while(impactedInCurrentRow && canMoveBoxes)
    {
        prevRow = currentRow;
        currentRow += rowDir;
        impactedInCurrentRow = false;
        for (int col = 0; col < 2*ROOM_DIM; col++)
        {
            if (impactedBoxSquares[prevRow][col])
            {
                if (DoubledMapGrid[currentRow][col] == '[')
                {
                    impactedBoxSquares[currentRow][col] = true;
                    impactedBoxSquares[currentRow][col + 1] = true;
                    impactedInCurrentRow = true;
                }
                else if (DoubledMapGrid[currentRow][col] == ']')
                {
                    impactedBoxSquares[currentRow][col - 1] = true;
                    impactedBoxSquares[currentRow][col] = true;
                    impactedInCurrentRow = true;
                }
                else if (DoubledMapGrid[currentRow][col] == '#')
                {
                    canMoveBoxes = false;
                }
            }
        }
    }

    // for (int i = 0; i < ROOM_DIM; i++)
    // {
    //     for (int j = 0; j < ROOM_DIM * 2; j++)
    //     {
    //         printf("%d", impactedBoxSquares[i][j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    // printf("canMoveBoxes is %s\n", canMoveBoxes ? "True" : "False");

    // printf("  currentRow: %d, robotRow: %d\n", currentRow, *robotRow);
    /*
     * We've already checked the currentRow in relevant spots and found no
     * walls. Time to move!
     */
    if (canMoveBoxes)
    {
        while (currentRow - rowDir != *robotRow)
        {
            // printf("  currentRow: %d\n", currentRow);
            for (int col = 0; col < 2 * ROOM_DIM; col++)
            {
                /*
                 * If there is an impacted box square, shift it in the direction
                 * of rowDir
                 */
                if (impactedBoxSquares[currentRow - rowDir][col])
                {
                    DoubledMapGrid[currentRow][col] = DoubledMapGrid[currentRow - rowDir][col];
                }
                else if (impactedBoxSquares[currentRow][col])
                {
                    DoubledMapGrid[currentRow][col] = '.';
                }
            }
            currentRow -= rowDir;
        }
        /* Boxes are placed appropriately. Just need to move the robot */
        if (DoubledMapGrid[*robotRow + rowDir][currentCol] == '[')
        {
            DoubledMapGrid[*robotRow + rowDir][currentCol + 1] = '.';
        }
        else
        {
            DoubledMapGrid[*robotRow + rowDir][currentCol - 1] = '.';
        }
        DoubledMapGrid[*robotRow + rowDir][currentCol] = '@';
        DoubledMapGrid[*robotRow][currentCol] = '.';
        *robotRow = *robotRow + rowDir;
    }
}

// [][][]
//  [][]
//   []
//   @
void getDir(char dir, int *rowDir, int *colDir)
{
    switch(dir)
    {
        case '^':
            *rowDir = -1;
            *colDir = 0;
            break;
        case '>':
            *rowDir = 0;
            *colDir = 1;
            break;
        case 'v':
            *rowDir = 1;
            *colDir = 0;
            break;
        case '<':
            *rowDir = 0;
            *colDir = -1;
            break;
        default:
            break;
    }
}