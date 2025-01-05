#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAZE_DIM 71
#define NUM_DIRS 4
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

void getMazeCosts(int row, int col, int candidateCost);
bool isInBounds (int row, int col);
bool dijkstras();

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    /* Set the array to 0, just in case */
    memset (Obstacles, 0, sizeof(Obstacles));
    memset (IsNodeVisited, 0, sizeof(IsNodeVisited));

    /* Set all the costs as -1 (unreachable) to start */
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
    while (getline(&line, &maxLen, filePointer) != -1 &&
           (currentIteration < 1024))
    {
        /* Number before the comma is the column, after is the row */
        colStr = strsep(&line, ",");
        Obstacles[atoi(line)][atoi(colStr)] = true;

        printf("%d,%d\n", atoi(colStr), atoi(line));

        currentIteration++;
    }

    bool isFinishReachable = true;
    while (isFinishReachable && getline(&line, &maxLen, filePointer) != -1)
    {
        colStr = strsep(&line, ",");
        Obstacles[atoi(line)][atoi(colStr)] = true;
        printf("%d,%d\n", atoi(colStr), atoi(line));

        memset (IsNodeVisited, 0, sizeof(IsNodeVisited));
        for (int i = 0; i < MAZE_DIM; i++)
        {
            for (int j = 0; j < MAZE_DIM; j++)
            {
                MazeCosts[i][j] = -1;
            }
        }
        MazeCosts[0][0] = 0;

        while (!dijkstras())
        {
            /* do nothing */
        }

        if (MazeCosts[MAZE_DIM - 1][MAZE_DIM - 1] == -1)
        {
            isFinishReachable = false;
        }
    }

    for (int i = 0; i < MAZE_DIM; i++)
    {
        for (int j = 0; j < MAZE_DIM; j++)
        {
            printf("%s", Obstacles[i][j] ? "#" : ".");
        }
        printf("\n");
    }

    MazeCosts[0][0] = 0;

    while (!dijkstras())
    {
        /* do nothing */
    }

    for (int i = 0; i < MAZE_DIM; i++)
    {
        for (int j = 0; j < MAZE_DIM; j++)
        {
            printf("%*d ", 2, MazeCosts[i][j]);
        }
        printf("\n");
    }

    printf("cost is %d\n", MazeCosts[MAZE_DIM - 1][MAZE_DIM - 1]);
    printf("last coordinate read was %s,%s\n", colStr, line);
}

bool dijkstras()
{
    /* Get the current node: unvisited one with the smallest distance */
    int minDist = -1;
    int minRow, minCol;
    for (int i = 0; i < MAZE_DIM; i++)
    {
        for (int j = 0; j < MAZE_DIM; j++)
        {
            if (!IsNodeVisited[i][j] &&
                !Obstacles[i][j] &&
                (MazeCosts[i][j] != -1 &&
                (MazeCosts[i][j] < minDist || minDist == -1)))
            {
                minDist = MazeCosts[i][j];
                minRow = i;
                minCol = j;
            }
        }
    }
    // printf("current node is %d, %d\n", minRow, minCol);

    /* We're at the target node */
    if (minRow == MAZE_DIM - 1 && minCol == MAZE_DIM - 1)
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

        if (isInBounds(newRow, newCol) && !Obstacles[newRow][newCol] &&
            (MazeCosts[newRow][newCol] == -1 ||
             MazeCosts[minRow][minCol] + 1 < MazeCosts[newRow][newCol]))
        {
            MazeCosts[newRow][newCol] = MazeCosts[minRow][minCol] + 1;
        }
    }

    /* Mark current node as visited */
    IsNodeVisited[minRow][minCol] = true;

    return false;
}

bool isInBounds (int row, int col)
{
    return (0 <= row && row < MAZE_DIM && 0 <= col && col < MAZE_DIM);
}

void getMazeCosts(int row, int col, int candidateCost)
{

    // if (!isInBounds(row, col) ||
    //     Obstacles[row][col])
    // {
    //     /* Base case: this square is out of bounds or contains an obstacle. Just return */
    //     return;
    // }
    printf("at position: %d, %d with cost %d\n", row, col, candidateCost);
    if ((MazeCosts[row][col] != -1) &&
        (MazeCosts[row][col] < candidateCost))
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
        MazeCosts[row][col] = candidateCost;

        /* Move in all four directions */
        if(isInBounds(row + 1, col) && !Obstacles[row + 1][col])
        {
            getMazeCosts(row + 1, col, candidateCost + 1);
        }
        if(isInBounds(row - 1, col) && !Obstacles[row - 1][col])
        {
            getMazeCosts(row - 1, col, candidateCost + 1);
        }
        if(isInBounds(row, col + 1) && !Obstacles[row][col + 1])
        {
            getMazeCosts(row, col + 1, candidateCost + 1);
        }
        if(isInBounds(row, col - 1) && !Obstacles[row][col - 1])
        {
            getMazeCosts(row, col - 1, candidateCost + 1);
        }
    }
}

