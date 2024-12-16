#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct freeSpaceNode_s
{
    int size;
    int startIdxInDefrag;
    struct freeSpaceNode_s *next;
} freeSpaceNode_t;

freeSpaceNode_t *createFreeList(char *diskDesc, int len);
long long moveBlockIfPossible(freeSpaceNode_t *head, int sizeToMove, long long id);

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

    long long totalSize = 0;
    for (int i = 0; i < inputSize - 1; i++)
    {
        // printf("%d", (line[i] - '0'));
        totalSize += (line[i] - '0');
    }
    printf("totalSize: %lld\n", totalSize);
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
    //     // printf("%d ", hardDrive[i]);
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
                    // printf("%d ", currentEndId);
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
                // printf("%d ", currentStartId);
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
        // printf("%d ", currentEndId);
    }

    // printf("currentEndPtrBlocks %d\n", currentEndPtrBlocks);
    printf("total is %lld\n", total);

    freeSpaceNode_t *freeListHead = createFreeList(line, inputSize - 1);
    freeSpaceNode_t *iterator = freeListHead;

    // while (iterator != NULL)
    // {
    //     printf("size %d, startIdx %d\n", iterator->size, iterator->startIdxInDefrag);
    //     iterator = iterator->next;
    // }

    endPtr = line + (inputSize - 2);
    long long currentId = (inputSize - 2) / 2;
    long long endPtrPosInDefrag = totalSize;
    long long totalPart2 = 0;
    long long movedContributions = 0;
    bool pointingAtOccupiedBlock = true;
    while (endPtr >= line)
    {
        // printf("endPtr is pointing at %c, id is %d\n", *endPtr, currentId);
        if (pointingAtOccupiedBlock)
        {
            /* Iterate over the free list to see if we can move this block */
            movedContributions = moveBlockIfPossible(freeListHead,
                                                     *endPtr - '0', currentId);
            if (movedContributions == 0)
            {
                /*
                 * We couldn't move this block. Calculate its contribution to
                 * the total in place
                 */
                long long saved = totalPart2;
                for (long long i = *endPtr - '0'; i > 0; i--)
                {
                    printf("  multiplying %lld * %lld\n", currentId, endPtrPosInDefrag - i);
                    totalPart2 += (long long) currentId * (endPtrPosInDefrag - i);
                }
                printf("couldn't move id %lld, contributed %lld\n", currentId,
                       totalPart2 - saved);

            }
            else
            {
                printf("moved id %lld, movedContributions are %lld\n",
                       currentId, movedContributions);
                totalPart2 += movedContributions;
            }
            currentId--;

            printf("total so far: %lld\n", totalPart2);
        }
        pointingAtOccupiedBlock = !pointingAtOccupiedBlock;
        endPtrPosInDefrag -= (*endPtr - '0');
        endPtr--;
    }

    printf("total for part 2 is %lld\n", totalPart2);
    // int totalSize = 0;
    // for (int i = 0; i < inputSize - 1; i++)
    // {
    //     totalSize += (line[i] - '0');
    // }
    // printf("totalSize: %d\n", totalSize);

/*
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
*/
}

long long moveBlockIfPossible(freeSpaceNode_t *head, int sizeToMove, long long id)
{
    long long retVal = 0;
    while (head != NULL && head->size < sizeToMove)
    {
        head = head->next;
    }
    if (head != NULL)
    {
        /* head points at the first freelist node that had enough space */
        for (int idx = head->startIdxInDefrag;
             idx < head->startIdxInDefrag + sizeToMove; idx++)
        {
            retVal += idx * id;
        }
        head->size -= sizeToMove;
        head->startIdxInDefrag += sizeToMove;
    }
    return retVal;
}

freeSpaceNode_t *createFreeList(char *diskDesc, int len)
{
    int curPosInDefrag = 0;
    freeSpaceNode_t *previous;
    freeSpaceNode_t *head;
    for (int i = 0; i < len; i++)
    {
        if (i % 2 == 1 && diskDesc[i] != '0')
        {
            freeSpaceNode_t *newNode = malloc(sizeof(freeSpaceNode_t));
            if (i == 1)
            {
                head = newNode;
            }
            newNode->size = diskDesc[i] - '0';
            newNode->startIdxInDefrag = curPosInDefrag;
            newNode->next = NULL;
            previous->next = newNode;
            previous = newNode;
        }
        curPosInDefrag += diskDesc[i] - '0';
    }
    return head;
}