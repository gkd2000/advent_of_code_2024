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

    /* Read the whole file into a string */
    char *line = NULL;
    size_t len = 0;
    int inputSize = getline(&line, &len, filePointer);
    // int idsSeen[10000] = {0};

    // int totalSize = 0;
    // for (int i = 0; i < inputSize - 1; i++)
    // {
    //     // printf("%d", (line[i] - '0'));
    //     totalSize += (line[i] - '0');
    // }
    // printf("totalSize: %d\n", totalSize);
    // int *hardDrive = malloc(totalSize * sizeof(int));
    // int hardDriveIdx = 0;
    // bool freeSpace = false;
    // int id = 0;
    // for (int i = 0; i < totalSize; i++)
    // {
    //     for (int j = 0; j < (line[i] - '0'); j++)
    //     {
    //         if (freeSpace)
    //         {
    //             hardDrive[hardDriveIdx] = -1;
    //         }
    //         else
    //         {
    //             hardDrive[hardDriveIdx] = id;
    //         }
    //         hardDriveIdx++;
    //     }
    //     if (freeSpace)
    //     {
    //         id++;
    //     }
    //     freeSpace = !freeSpace;
    // }

    // int freeSpaceIdx = 0;
    // int endIdx = totalSize - 1;
    // while (freeSpaceIdx  < endIdx)
    // {
    //     /* Advance freeSpaceIdx so it's pointing at free space */
    //     while (hardDrive[freeSpaceIdx] != -1)
    //     {
    //         freeSpaceIdx++;
    //     }
    //     /* Move back endIdx so it's pointing at data */
    //     while (hardDrive[endIdx] == -1)
    //     {
    //         endIdx--;
    //     }

    //     /* Fill free space from the end */
    //     if (freeSpaceIdx < endIdx)
    //     {
    //         hardDrive[freeSpaceIdx] = hardDrive[endIdx];
    //         hardDrive[endIdx] = -1;
    //         freeSpaceIdx++;
    //         endIdx--;
    //     }
    // }

    // long long total = 0;
    // for (int i = 0; i < totalSize && hardDrive[i] != -1; i++)
    // {
    //     printf("%d ", hardDrive[i]);
    //     total += i * hardDrive[i];
    // }
    // printf("\ntotal: %lld\n", total);

    char *startPtr = line;
    char *endPtr = line + (inputSize - 2); /* Skip over the null terminator and newline char */

    // printf("len: %d, %c %c\n", inputSize, *startPtr, *endPtr);
    long long total = 0;
    int numBlocks = 0;
    bool fillingEmptySpace = false;
    int currentStartId = 0;
    int currentEndId = (inputSize - 2) / 2;
    int currentPosInDefrag = 0;
    int spaceToFill = 0;
    int currentEndPtrBlocks = (*endPtr) - '0';
    while (startPtr < endPtr)
    {
        if (fillingEmptySpace)
        {
            spaceToFill = (*startPtr) - '0';
            for (int i = 0; i < spaceToFill; i++)
            {
                if (currentEndPtrBlocks == 0)
                {
                    endPtr -= 2;
                    currentEndPtrBlocks = (*endPtr) - '0';
                    currentEndId--;
                }
                if (startPtr < endPtr)
                {
                    total += currentEndId * currentPosInDefrag;
                    currentEndPtrBlocks--;
                    currentPosInDefrag++;
                    printf("%d ", currentEndId);
                    // idsSeen[currentEndId] += 1;
                }
                else
                {
                    currentEndPtrBlocks = 0;
                }
            }
        }
        else
        {
            numBlocks = (*startPtr) - '0';
            for (int i = 0; i < numBlocks; i++)
            {
                total += currentStartId * currentPosInDefrag;
                currentPosInDefrag++;
                printf("%d ", currentStartId);
                // idsSeen[currentStartId] += 1;
            }
            currentStartId++;
        }
        startPtr++;
        fillingEmptySpace = !fillingEmptySpace;
    }

    while (currentEndPtrBlocks > 0)
    {
        total += currentEndId * currentPosInDefrag;
        currentEndPtrBlocks--;
        currentPosInDefrag++;
        // idsSeen[currentEndId] += 1;
        printf("%d ", currentEndId);
    }

    printf("currentEndPtrBlocks %d\n", currentEndPtrBlocks);
    printf("\ntotal is %lld\n", total);

    int totalSize = 0;
    for (int i = 0; i < inputSize - 1; i++)
    {
        // printf("%d", (line[i] - '0'));
        totalSize += (line[i] - '0');
    }
    printf("totalSize: %d\n", totalSize);
    int *hardDrive = malloc(totalSize * sizeof(int));
    int hardDriveIdx = 0;
    bool freeSpace = false;
    int id = 0;
    for (int i = 0; i < totalSize; i++)
    {
        for (int j = 0; j < (line[i] - '0'); j++)
        {
            if (freeSpace)
            {
                hardDrive[hardDriveIdx] = -1;
            }
            else
            {
                hardDrive[hardDriveIdx] = id;
            }
            hardDriveIdx++;
        }
        if (freeSpace)
        {
            id++;
        }
        freeSpace = !freeSpace;
    }

    *startPtr = line + 1;
    *endPtr = line + (inputSize - 2);
    int blocksToMove = 0;
    int freeBlocks = 0;
    int hardDriveIdx = *line;
    // int lastCountIdx = 0;
    while (endPtr >= 0)
    {
        blocksToMove = (*endPtr) - '0';
        startPtr = line + 1;
        while (startPtr < endPtr)
        {
            freeBlocks = (*startPtr) - '0';
            if (freeBlocks >= blocksToMove)
            {
                for (int i = 0; i < blocksToMove; i++)
                {
                    hardDrive[hardDriveIdx]
                }
            }
        }
    }

}