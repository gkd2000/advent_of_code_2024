#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAZE_DIM 141
#define NUM_DIRS 4
char Maze[MAZE_DIM][MAZE_DIM];
long MazeCostsFacingEast[MAZE_DIM][MAZE_DIM];
long MazeCostsFacingWest[MAZE_DIM][MAZE_DIM];
long MazeCostsFacingNorth[MAZE_DIM][MAZE_DIM];
long MazeCostsFacingSouth[MAZE_DIM][MAZE_DIM];
bool BestPaths[MAZE_DIM][MAZE_DIM];

void getMazeCosts(int row, int col, int rowDir, int colDir, long long thisCost);
void printCostMatrix(long *costMatrix);
void findBestPaths(long *costMatrix, int row, int col, long prevCost);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    /* Set the array to 0, just in case */
    memset (Maze, 0, sizeof(Maze));
    memset(BestPaths, 0, sizeof(BestPaths));

    /* Set all the costs as -1 (unreachable) to start */
    for (int i = 0; i < MAZE_DIM; i++)
    {
        for (int j = 0; j < MAZE_DIM; j++)
        {
            MazeCostsFacingEast[i][j] = -1;
            MazeCostsFacingWest[i][j] = -1;
            MazeCostsFacingNorth[i][j] = -1;
            MazeCostsFacingSouth[i][j] = -1;
        }
    }

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
    bool foundStart = false;
    bool foundEnd = false;
    int startRow, startCol, endRow, endCol;
    char *findPtr;
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1)
    {
        /* Copy maze */
        memcpy (&Maze[currentRow][0], line, MAZE_DIM);

        if (!foundStart && (findPtr = strchr(line, 'S')) != NULL)
        {
            startRow = currentRow;
            startCol = findPtr - line;
            foundStart = true;
        }

        if (!foundEnd && (findPtr = strchr(line, 'E')) != NULL)
        {
            endRow = currentRow;
            endCol = findPtr - line;
            foundEnd = true;
        }
        currentRow++;
    }

    int rowDir = 0;
    int colDir = 1;

    getMazeCosts(startRow, startCol, rowDir, colDir, 0);

    // printCostMatrix(&MazeCostsFacingEast[0][0]);
    // printCostMatrix(&MazeCostsFacingWest[0][0]);
    // printCostMatrix(&MazeCostsFacingSouth[0][0]);
    // printCostMatrix(&MazeCostsFacingNorth[0][0]);

    long leastCosts[MAZE_DIM][MAZE_DIM];
    long smallestSoFar = -1;
    for (int i = 0; i < MAZE_DIM; i++)
    {
        for (int j = 0; j < MAZE_DIM; j++)
        {
            smallestSoFar = -1;
            if (MazeCostsFacingEast[i][j] != -1)
            {
                smallestSoFar = MazeCostsFacingEast[i][j];
            }
            if (smallestSoFar == -1 || (MazeCostsFacingWest[i][j] != -1 && MazeCostsFacingWest[i][j] < smallestSoFar))
            {
                smallestSoFar = MazeCostsFacingWest[i][j];
            }
            if (smallestSoFar == -1 || (MazeCostsFacingSouth[i][j] != -1 && MazeCostsFacingSouth[i][j] < smallestSoFar))
            {
                smallestSoFar = MazeCostsFacingSouth[i][j];
            }
            if (smallestSoFar == -1 || (MazeCostsFacingNorth[i][j] != -1 && MazeCostsFacingNorth[i][j] < smallestSoFar))
            {
                smallestSoFar = MazeCostsFacingNorth[i][j];
            }
            leastCosts[i][j] = smallestSoFar;
        }
    }

    // printCostMatrix(&leastCosts[0][0]);

    printf("end costs are: %ld, %ld, %ld, %ld\n",
           MazeCostsFacingEast[endRow][endCol],
           MazeCostsFacingWest[endRow][endCol],
           MazeCostsFacingNorth[endRow][endCol],
           MazeCostsFacingSouth[endRow][endCol]);

    long lowestCost = leastCosts[endRow][endCol];
    if (lowestCost == MazeCostsFacingEast[endRow][endCol])
    {
        findBestPaths(&MazeCostsFacingEast[0][0], endRow, endCol, lowestCost + 1);
    }
    if (lowestCost == MazeCostsFacingWest[endRow][endCol])
    {
        findBestPaths(&MazeCostsFacingWest[0][0], endRow, endCol, lowestCost + 1);
    }
    if (lowestCost == MazeCostsFacingNorth[endRow][endCol])
    {
        findBestPaths(&MazeCostsFacingNorth[0][0], endRow, endCol, lowestCost + 1);
    }
    if (lowestCost == MazeCostsFacingSouth[endRow][endCol])
    {
        findBestPaths(&MazeCostsFacingSouth[0][0], endRow, endCol, lowestCost + 1);
    }

    int totalBestSeats = 0;
    for (int i = 1; i < MAZE_DIM - 1; i++)
    {
        for (int j = 1; j < MAZE_DIM - 1; j++)
        {
            if (BestPaths[i][j])
            {
                totalBestSeats++;
            }
        }
    }

    printf("total best seats is %d\n", totalBestSeats);
}

/*
 * Start in cost matrix that had the winning cost. Look at adjacent positions
 * in all other cost matrices. For all the ones that are smaller, branch
 * and continue. Mark each position visited in BestPaths.
 */
void findBestPaths(long *costMatrix, int row, int col, long prevCost)
{
    if ((costMatrix[row * MAZE_DIM + col] == -1) ||
        (prevCost - costMatrix[row * MAZE_DIM + col] != 1 &&
         prevCost - costMatrix[row * MAZE_DIM + col] != 1001))
    {
        /*
         * Base case. No path here in the provided matrix, or it's not on the
         * path to the end
         */
        return;
    }

    if (costMatrix[row * MAZE_DIM + col] == 0)
    {
        /* Base case: at the start */
        BestPaths[row][col] = true;
        return;
    }

    /*
     * Recursive step. Mark this square as on a winning path and look in all
     * directions in all cost matrices
     */
    BestPaths[row][col] = true;
    int newRows[NUM_DIRS] = {row - 1, row + 1, row, row};
    int newCols[NUM_DIRS] = {col, col, col - 1, col + 1};
    int rowDir, colDir;
    long thisCost = costMatrix[row * MAZE_DIM + col];

    for (int i = 0; i < NUM_DIRS; i++)
    {
        findBestPaths(&MazeCostsFacingEast[0][0], newRows[i],
                      newCols[i], thisCost);
        findBestPaths(&MazeCostsFacingWest[0][0], newRows[i],
                      newCols[i], thisCost);
        findBestPaths(&MazeCostsFacingNorth[0][0], newRows[i],
                      newCols[i], thisCost);
        findBestPaths(&MazeCostsFacingSouth[0][0], newRows[i],
                      newCols[i], thisCost);
    }
}

void printCostMatrix(long *costMatrix)
{
    for (int i = 1; i < MAZE_DIM - 1; i++)
    {
        for (int j = 1; j < MAZE_DIM - 1; j++)
        {
            if (costMatrix[i * MAZE_DIM + j] == -1)
            {
                printf("   .   ");
            }
            else
            {
                printf("%*ld ", 6, costMatrix[i * MAZE_DIM + j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void getMazeCosts(int row, int col, int rowDir, int colDir, long long thisCost)
{
    long *relevantCostMatrix;
    if (rowDir == 0 && colDir == 1)
    {
        relevantCostMatrix = &MazeCostsFacingEast[0][0];
    }
    else if (rowDir == 0 && colDir == -1)
    {
        relevantCostMatrix = &MazeCostsFacingWest[0][0];
    }
    else if (rowDir == 1 && colDir == 0)
    {
        relevantCostMatrix = &MazeCostsFacingSouth[0][0];
    }
    else if (rowDir == -1 && colDir == 0)
    {
        relevantCostMatrix = &MazeCostsFacingNorth[0][0];
    }

    if (Maze[row][col] == '#')
    {
        /* Base case: this square is unreachable. Just return */
        return;
    }
    else if ((relevantCostMatrix[row * MAZE_DIM + col] != -1) &&
             (relevantCostMatrix[row * MAZE_DIM + col] < thisCost))
    {
        /* Base case: there is a cheaper way to get here. Just return */
        return;
    }
    else
    {
        /*
         * This tile is not a wall. It is either unvisted, or we have found
         * a cheaper way to get there than we previously realized. Update the
         * cost matrix and make recursive calls.
         */
        relevantCostMatrix[row * MAZE_DIM + col] = thisCost;
        /* Move without turning, incurring a cost of 1 */
        getMazeCosts(row + rowDir, col + colDir, rowDir, colDir, thisCost + 1);
        /* Turn right or left then move, incurring a cost of 1001 */
        if (rowDir == 0)
        {
            getMazeCosts(row + 1, col, 1, 0, thisCost + 1001);
            getMazeCosts(row - 1, col, -1, 0, thisCost + 1001);
        }
        else
        {
            getMazeCosts(row, col + 1, 0, 1, thisCost + 1001);
            getMazeCosts(row, col - 1, 0, -1, thisCost + 1001);
        }
    }
}

