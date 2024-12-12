#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define GRID_DIM 41
char TopoMap[GRID_DIM][GRID_DIM];
bool FoundPathAlready[GRID_DIM][GRID_DIM];

int findReachablePeaks(int row, int col);
int findReachablePeaksKernel (int row, int col, char prevVal);
int findDistinctPaths(int row, int col);
int findDistinctPathsKernel (int row, int col, char prevVal);
bool isInBounds (int row, int col);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    /* Attempt to open the file */
    char *fileName = argv[1];
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    /* Read the whole file into a 2D array, preserving structure */
    char *line = NULL;
    size_t len = 0;
    int currentRow = 0;
    while (getline(&line, &len, filePointer) != -1)
    {
        // printf("%zu\n", len);
        memcpy (&TopoMap[currentRow][0], line, GRID_DIM);
        currentRow++;
    }

    int totalScore = 0;
    int totalRating = 0;
    for (int row = 0; row < GRID_DIM; row++)
    {
        for (int col = 0; col < GRID_DIM; col++)
        {
            if (TopoMap[row][col] == '0')
            {
                totalScore += findReachablePeaks(row, col);
                totalRating += findDistinctPaths(row, col);
            }
        }
    }

    printf("Total score is %d\n", totalScore);
    printf("Total rating is %d\n", totalRating);
}

int findReachablePeaks(int row, int col)
{
    memset (FoundPathAlready, 0, sizeof(FoundPathAlready));
    return findReachablePeaksKernel(row, col + 1, '0') +
           findReachablePeaksKernel(row, col - 1, '0') +
           findReachablePeaksKernel(row + 1, col, '0') +
           findReachablePeaksKernel(row - 1, col, '0');
}

int findReachablePeaksKernel (int row, int col, char prevVal)
{
    if ((!isInBounds(row, col)) || (TopoMap[row][col] - prevVal != 1))
    {
        /*
         * Base case: out of bounds index or not an increase of 1 from the
         * previous value
         */
        return 0;
    }
    else if ((TopoMap[row][col] == '9') && (FoundPathAlready[row][col]))
    {
        /* Base case: we've already found this path */
        return 0;
    }
    else if (TopoMap[row][col] == '9')
    {
        /* Base case: path found */
        FoundPathAlready[row][col] = true;
        return 1;
    }
    else
    {
        /* Recursion time */
        char currentVal = TopoMap[row][col];
        return findReachablePeaksKernel(row, col + 1, currentVal) +
               findReachablePeaksKernel(row, col - 1, currentVal) +
               findReachablePeaksKernel(row + 1, col, currentVal) +
               findReachablePeaksKernel(row - 1, col, currentVal);
    }
}

int findDistinctPaths(int row, int col)
{
    memset (FoundPathAlready, 0, sizeof(FoundPathAlready));
    return findDistinctPathsKernel(row, col + 1, '0') +
           findDistinctPathsKernel(row, col - 1, '0') +
           findDistinctPathsKernel(row + 1, col, '0') +
           findDistinctPathsKernel(row - 1, col, '0');
}

int findDistinctPathsKernel (int row, int col, char prevVal)
{
    if ((!isInBounds(row, col)) || (TopoMap[row][col] - prevVal != 1))
    {
        /*
         * Base case: out of bounds index or not an increase of 1 from the
         * previous value
         */
        return 0;
    }
    else if (TopoMap[row][col] == '9')
    {
        /* Base case: path found */
        FoundPathAlready[row][col] = true;
        return 1;
    }
    else
    {
        /* Recursion time */
        char currentVal = TopoMap[row][col];
        return findDistinctPathsKernel(row, col + 1, currentVal) +
               findDistinctPathsKernel(row, col - 1, currentVal) +
               findDistinctPathsKernel(row + 1, col, currentVal) +
               findDistinctPathsKernel(row - 1, col, currentVal);
    }
}

bool isInBounds (int row, int col)
{
    return (row >= 0) && (row < GRID_DIM) && (col >= 0) && (col < GRID_DIM);
}