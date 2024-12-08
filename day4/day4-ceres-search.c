#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define NUM_ROWS    140
#define NUM_COLUMNS 140
#define NUM_DIRECTIONS 8
char WordGrid[NUM_ROWS][NUM_COLUMNS];
char Directions[NUM_DIRECTIONS][2] = 
{
    {-1,  0}, /* North     */
    {-1,  1}, /* NorthEast */
    { 0,  1}, /* East      */
    { 1,  1}, /* SouthEast */
    { 1,  0}, /* South     */
    { 1, -1}, /* SouthWest */
    { 0, -1}, /* West      */
    {-1, -1}  /* NorthWest */
};
// enum DirectionIndices
// {
//     North,
//     NorthEast,
//     East,
//     SouthEast,
//     South,
//     SouthWest,

// };

bool findWord(int row, int col, int dir);
bool findXmasShape(int row, int col);

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
        memcpy (&WordGrid[currentRow][0], line, NUM_COLUMNS);
        currentRow++;
    }

    int wordsFound = 0;
    int shapesFound = 0;
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLUMNS; j++)
        {
            printf("%c", WordGrid[i][j]);
            if (WordGrid[i][j] == 'X')
            {
                // printf(" Found X\n");
                for (int dir = 0; dir < NUM_DIRECTIONS; dir++)
                {
                    if (findWord(i, j, dir))
                    {
                        wordsFound++;
                        // printf("%d, %d, %d\n", i, j, dir);
                    }
                }
            }
            else if (WordGrid[i][j] == 'A')
            {
                if (findXmasShape(i, j))
                {
                    shapesFound++;
                }
            }
        }
        printf("\n");
    }

    printf("found %d occurrences of XMAS\n", wordsFound);
    printf("found %d occurrences of X-MAS\n", shapesFound);
}

/*
 * Given a starting row, column, and direction, determine whether the
 * word XMAS appears in that direction. Assumes that WordGrid[row][col]
 * is an 'X', and that dir is an index into the Directions array
 */
bool findWord(int row, int col, int dir)
{
    int rowDir = Directions[dir][0];
    int colDir = Directions[dir][1];
    int edgeRow = row + (3 * rowDir);
    int edgeCol = col + (3 * colDir);

    // printf("dir: %d, rowDir: %d, colDir: %d, edgeRow: %d, edgeCol: %d\n",
    //        dir, rowDir, colDir, edgeRow, edgeCol);
    /*
     * Make sure that the index where we will look for 'S' in in bounds, then
     * check the next three entries in the specified direction.
     */
    // printf("edgeRow in bounds: %d\n%d\n%d\n%d\n")
    return ( (edgeRow >= 0) && (edgeRow < NUM_ROWS) && (edgeCol >= 0) &&
             (edgeCol < NUM_COLUMNS) &&
             (WordGrid[row + rowDir][col + colDir] == 'M') &&
             (WordGrid[row + (2 * rowDir)][col + (2 * colDir)] == 'A') &&
             (WordGrid[edgeRow][edgeCol] == 'S') );
}

bool findXmasShape(int row, int col)
{
    return ( (row - 1 >= 0) && (row + 1 < NUM_ROWS) && (col - 1 >= 0) &&
             (col + 1 < NUM_COLUMNS) &&
             ( ((WordGrid[row-1][col-1] == 'M') && (WordGrid[row+1][col+1] == 'S')) ||
               ((WordGrid[row-1][col-1] == 'S') && (WordGrid[row+1][col+1] == 'M')) ) &&
             ( ((WordGrid[row-1][col+1] == 'M') && (WordGrid[row+1][col-1] == 'S')) ||
               ((WordGrid[row-1][col+1] == 'S') && (WordGrid[row+1][col-1] == 'M')) ) );
}