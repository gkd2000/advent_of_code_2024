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
long long moveBlockIfPossible(freeSpaceNode_t *head, int sizeToMove,
                              long long id, long long currentIdxInDefrag);

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

    long long totalSize = 0;
    for (int i = 0; i < inputSize - 1; i++)
    {
        totalSize += (line[i] - '0');
    }

    char *startPtr = line;
    char *endPtr = line + (inputSize - 2); /* Skip over the null terminator and newline char */

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
    }

    printf("total for part 1 is %lld\n", total);

    freeSpaceNode_t *freeListHead = createFreeList(line, inputSize - 1);

    endPtr = line + (inputSize - 2);
    long long currentId = (inputSize - 2) / 2;
    long long endPtrPosInDefrag = totalSize;
    long long totalPart2 = 0;
    long long movedContributions = 0;
    bool pointingAtOccupiedBlock = true;
    while (endPtr >= line)
    {
        if (pointingAtOccupiedBlock)
        {
            /* Iterate over the free list to see if we can move this block */
            movedContributions = moveBlockIfPossible(freeListHead,
                                                     *endPtr - '0', currentId,
                                                     endPtrPosInDefrag);
            if (movedContributions == 0)
            {
                /*
                 * We couldn't move this block. Calculate its contribution to
                 * the total in place
                 */
                long long saved = totalPart2;
                for (long long i = *endPtr - '0'; i > 0; i--)
                {
                    totalPart2 += (long long) currentId * (endPtrPosInDefrag - i);
                }

            }
            else
            {
                totalPart2 += movedContributions;
            }
            currentId--;
        }
        pointingAtOccupiedBlock = !pointingAtOccupiedBlock;
        endPtrPosInDefrag -= (*endPtr - '0');
        endPtr--;
    }

    printf("total for part 2 is %lld\n", totalPart2);
}

long long moveBlockIfPossible(freeSpaceNode_t *head, int sizeToMove,
                              long long id, long long currentIdxInDefrag)
{
    long long retVal = 0;
    long ctr = 0;
    while (head != NULL && head->size < sizeToMove)
    {
        head = head->next;
        ctr++;
    }

    /*
     * Make sure that if we found a freelist node, it was before the index
     * currently occupied by this block.
     */
    if (head != NULL && head->startIdxInDefrag < currentIdxInDefrag)
    {
        /* head points at the first freelist node that had enough space */
        for (long long idx = head->startIdxInDefrag;
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
            if (newNode == NULL)
            {
                printf("malloc failure!\n");
            }
            if (i == 1)
            {
                head = newNode;
            }
            newNode->size = diskDesc[i] - '0';
            newNode->startIdxInDefrag = curPosInDefrag;
            newNode->next = NULL;
            if (previous != NULL)
            {
                previous->next = newNode;
            }
            previous = newNode;
        }
        curPosInDefrag += diskDesc[i] - '0';
    }
    return head;
}