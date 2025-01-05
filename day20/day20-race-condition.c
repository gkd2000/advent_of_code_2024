#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAZE_DIM 141
#define NUM_DIRS 4
#define MAX_CHEAT_SECS 20
bool Obstacles[MAZE_DIM][MAZE_DIM];
int MazeCosts[MAZE_DIM][MAZE_DIM];
bool IsNodeVisited[MAZE_DIM][MAZE_DIM];

int Directions[4][2] =
{
    {0, 1},
    {0, -1},
    {1, 0},
    {-1, 0}
};

bool isInBounds (int row, int col);
bool dijkstras(int endRow, int endCol, bool *visitedNodes, bool *obstacleMat,
               int *costMat);
int getSimpleCheats(int curRow, int curCol, int startRow, int startCol,
                    int *costMat);
long getBigCheats(int curRow, int curCol, int startRow, int startCol,
                  int cheatLen, int *costMat);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    /* Set the arrays to 0, just in case */
    memset (Obstacles, 0, sizeof(Obstacles));
    memset (IsNodeVisited, 0, sizeof(IsNodeVisited));

    /* Set all the costs as -1 (unreachable) */
    for (int i = 0; i < MAZE_DIM; i++)
    {
        for (int j = 0; j < MAZE_DIM; j++)
        {
            MazeCosts[i][j] = -1;
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
    int currentIteration = 0;
    char *colStr;
    int lineLen = 0;
    int currentLine = 0;
    int startRow, startCol, endRow, endCol;
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1)
    {
        for (int i = 0; i < MAZE_DIM; i++)
        {
            if (line[i] == '#')
            {
                Obstacles[currentLine][i] = true;
            }
            else if (line[i] == 'S')
            {
                startRow = currentLine;
                startCol = i;
            }
            else if (line[i] == 'E')
            {
                endRow = currentLine;
                endCol = i;
            }
        }
        currentLine++;
    }

    MazeCosts[startRow][startCol] = 0;

    /* Populate MazeCosts with shortest distances from start position */
    while(!dijkstras(endRow, endCol, &IsNodeVisited[0][0], &Obstacles[0][0],
                     &MazeCosts[0][0]))
    {

    }

    int numCheats = getSimpleCheats(endRow, endCol, startRow, startCol,
                                    &MazeCosts[0][0]);

    long numCheatsPt2 = getBigCheats(endRow, endCol, startRow, startCol,
                                     MAX_CHEAT_SECS, &MazeCosts[0][0]);

    printf("number of cheats for part 1 is %d, part 2 is %ld\n",
           numCheats, numCheatsPt2);
}

long getBigCheats(int curRow, int curCol, int startRow, int startCol,
                  int cheatLen, int *costMat)
{
    long totalCheats = 0;
    int rowDir, colDir, curCost, nextRowDir, nextColDir, adjCost, cheatCost,
        candRow, candCol, candCheatDuration;
    bool foundDir = false;
    while (curRow != startRow || curCol != startCol)
    {
        /* Find the next position on the shortest path */
        foundDir = false;
        curCost = costMat[curRow * MAZE_DIM + curCol];
        for (int i = 0; i < NUM_DIRS && !foundDir; i++)
        {
            rowDir = Directions[i][0];
            colDir = Directions[i][1];
            adjCost = costMat[((curRow + rowDir) * MAZE_DIM) + curCol + colDir];
            if (adjCost == curCost - 1)
            {
                foundDir = true;
                nextRowDir = rowDir;
                nextColDir = colDir;
            }
        }

        /*
         * Search positions a taxicab distance of 20 from our current square and
         * determine which (if any) of them are valid cheats.
         */
        for (int rowOffset = -cheatLen; rowOffset <= cheatLen; rowOffset++)
        {
            for (int colOffset = abs(rowOffset) - cheatLen;
                 colOffset <= cheatLen - abs(rowOffset); colOffset++)
            {
                candRow = curRow + rowOffset;
                candCol = curCol + colOffset;
                if (isInBounds(candRow, candCol))
                {
                    candCheatDuration = abs(rowOffset) + abs(colOffset);
                    cheatCost = costMat[candRow * MAZE_DIM + candCol];
                    if (cheatCost != -1 &&
                        cheatCost <= curCost - candCheatDuration - 100)
                    {
                        totalCheats++;
                    }
                }
            }
        }

        curRow += nextRowDir;
        curCol += nextColDir;
    }
    return totalCheats;
}

/*
 * Traverse the cost matrix back to the start along the most efficient path,
 * looking for cheats which last 2 picoseconds
 */
int getSimpleCheats(int curRow, int curCol, int startRow, int startCol, int *costMat)
{
    int rowDir, colDir, curCost, nextRowDir, nextColDir, adjCost, skipCost;
    bool foundDir = false;
    int totalCheats = 0;
    while (curRow != startRow || curCol != startCol)
    {
        /* Determine the next position on the shortest path and move to it */
        curCost = costMat[curRow * MAZE_DIM + curCol];
        foundDir = false;
        for (int i = 0; i < NUM_DIRS; i++)
        {
            rowDir = Directions[i][0];
            colDir = Directions[i][1];
            adjCost = costMat[((curRow + rowDir) * MAZE_DIM) + curCol + colDir];
            skipCost = costMat[((curRow + 2 * rowDir) * MAZE_DIM) + curCol + 2 * colDir];
            /* Check if there are any cheats from the current position */
            if (adjCost == -1 &&
                isInBounds(curRow + 2 * rowDir, curCol + 2 * colDir) &&
                skipCost < curCost - 2 && skipCost != -1 &&
                curCost - skipCost - 2 >= 100)
            {
                totalCheats++;
            }
            /* Update direction */
            else if (adjCost == curCost - 1)
            {
                foundDir = true;
                nextRowDir = rowDir;
                nextColDir = colDir;
            }
        }
        if (!foundDir)
        {
            printf("found no direction :(\n");
        }

        curRow += nextRowDir;
        curCol += nextColDir;
    }
    return totalCheats;
}

bool dijkstras(int endRow, int endCol, bool *visitedNodes, bool *obstacleMat,
               int *costMat)
{
    /* Get the current node: unvisited one with the smallest distance */
    int minDist = -1;
    int minRow, minCol;
    for (int i = 0; i < MAZE_DIM; i++)
    {
        for (int j = 0; j < MAZE_DIM; j++)
        {
            if (!visitedNodes[i * MAZE_DIM + j] &&
                !obstacleMat[i * MAZE_DIM + j] &&
                (costMat[i * MAZE_DIM + j] != -1 &&
                (costMat[i * MAZE_DIM + j] < minDist || minDist == -1)))
            {
                minDist = costMat[i * MAZE_DIM + j];
                minRow = i;
                minCol = j;
            }
        }
    }

    /* We're at the target node */
    if (minRow == endRow && minCol == endCol)
    {
        return true;
    }
    /* No nodes found */
    if (minDist == -1)
    {
        return true;
    }

    /* Consider all of the current node's neighbors, updating the cost if necessary */
    int newRow, newCol;
    for (int i = 0; i < NUM_DIRS; i++)
    {
        newRow = minRow + Directions[i][0];
        newCol = minCol + Directions[i][1];

        if (isInBounds(newRow, newCol) && !obstacleMat[newRow * MAZE_DIM + newCol] &&
            (costMat[newRow * MAZE_DIM + newCol] == -1 ||
             costMat[minRow * MAZE_DIM + minCol] + 1 < costMat[newRow * MAZE_DIM + newCol]))
        {
            costMat[newRow * MAZE_DIM + newCol] = costMat[minRow * MAZE_DIM + minCol] + 1;
        }
    }

    /* Mark current node as visited */
    visitedNodes[minRow * MAZE_DIM + minCol] = true;

    return false;
}

bool isInBounds (int row, int col)
{
    return (0 <= row && row < MAZE_DIM && 0 <= col && col < MAZE_DIM);
}
