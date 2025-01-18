#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SEQ_LEN 4
#define ARROW_START_ROW 0
#define ARROW_START_COL 2
#define NUM_ROBOTS_ON_ARROWS_PART1 2
#define NUM_ROBOTS_ON_ARROWS_PART2 25
#define MAX_KEYPRESS_SEQ_LEN 10

char *getFirstRobotMoves(char *target, int *currentRow, int *currentCol);
void getPosOnNumpad(char target, int *row, int *col);
char *pullbackArrowKeys(char *target);
void getPosOnArrowKeys(char target, int *row, int *col);
long long pullbackArrowKeysRecursive(char *target, int targetLen,
                                     int numRobotsLeft);
long long getSavedVal(char *target, int numRobotsLeft);
void addSavedVal(char *target, int targetStrLen, int numRobotsLeft,
                 long long resultLen);

typedef struct node
{
    char keySequence[MAX_KEYPRESS_SEQ_LEN];
    long long length;
    struct node *next;
} node_t;

/*
 * SavedLengths[i] is the head of a liked list containing the sequences and
 * their lengths when the number of robots left is i + 1
 */
node_t *SavedLengths[NUM_ROBOTS_ON_ARROWS_PART2];

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

    /* Read the input file and process codes one at a time */
    char *line = NULL;
    size_t maxLen = 0;
    int currentIteration = 0;
    char *colStr;
    int lineLen = 0;
    int currentLine = 0;
    int startRow = 3;
    int startCol = 2;
    char targetSequence[SEQ_LEN + 1];
    int codeNum = 0;
    int totalPart1 = 0;
    long long totalPart2 = 0;
    
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1)
    {
        memcpy(targetSequence, line, SEQ_LEN);
        targetSequence[SEQ_LEN] = '\0';

        char *firstRobotMoves = getFirstRobotMoves(targetSequence, &startRow,
                                                   &startCol);

        codeNum = atoi(targetSequence);

        totalPart1 +=
            codeNum * pullbackArrowKeysRecursive(firstRobotMoves,
                                                 strlen(firstRobotMoves),
                                                 NUM_ROBOTS_ON_ARROWS_PART1);
        totalPart2 +=
            codeNum * pullbackArrowKeysRecursive(firstRobotMoves,
                                                 strlen(firstRobotMoves),
                                                 NUM_ROBOTS_ON_ARROWS_PART2);

        free (firstRobotMoves);
    }

    printf("total for part 1 is %d, total for part 2 is %lld\n", totalPart1, totalPart2);

    // char *testStr = "^>A";
    // char *pullback1 = pullbackArrowKeys(testStr);
    // char *pullback2 = pullbackArrowKeys(pullback1);
    // char *pullback3 = pullbackArrowKeys(pullback2);
    // char *pullback4 = pullbackArrowKeys(pullback3);
    // printf("%s\n%s\n%s\n%s\n%s\n", testStr, pullback1, pullback2, pullback3,
    //        pullback4);
}

long long pullbackArrowKeysRecursive(char *target, int targetLen,
                                     int numRobotsLeft)
{
    int currentRow = ARROW_START_ROW;
    int currentCol = ARROW_START_COL;
    int targetRow, targetCol;
    char *ans = malloc(20);
    int ansIdx = 0;
    long long totalLen = 0;
    long long savedVal = 0;

    /* Base case: we're at the human typing */
    if (numRobotsLeft == 1)
    {
        char *buttonSeq = pullbackArrowKeys(target);
        long long retVal = strlen(buttonSeq);
        free(buttonSeq);
        return retVal;
    }

    /* Check if we've computed this value before */
    if ((savedVal = getSavedVal(target, numRobotsLeft)) != -1)
    {
        return savedVal;
    }

    /* Not base case: iterate over the target */
    for (int i = 0; i < targetLen; i++)
    {
        getPosOnArrowKeys(target[i], &targetRow, &targetCol);

        /* Need to go down and left, avoiding blank space */
        if (targetRow == 1 && targetCol == 0 && targetCol < currentCol)
        {
            memset(ans + ansIdx, 'v', targetRow - currentRow);
            ansIdx += targetRow - currentRow;

            memset(ans + ansIdx, '<', currentCol - targetCol);
            ansIdx += currentCol - targetCol;
        }
        /* Need to go down and left, but no need to avoid blank space */
        else if (targetCol <= currentCol && targetRow >= currentRow)
        {
            memset(ans + ansIdx, '<', currentCol - targetCol);
            ansIdx += currentCol - targetCol;

            memset(ans + ansIdx, 'v', targetRow - currentRow);
            ansIdx += targetRow - currentRow;
        }
        /* Need to go down and right */
        else if (targetCol >= currentCol && targetRow >= currentRow)
        {
            memset(ans + ansIdx, 'v', targetRow - currentRow);
            ansIdx += targetRow - currentRow;

            memset(ans + ansIdx, '>', targetCol - currentCol);
            ansIdx += targetCol - currentCol;
        }
        /* Need to go up and left */
        else if (targetCol <= currentCol && targetRow <= currentRow)
        {
            memset(ans + ansIdx, '<', currentCol - targetCol);
            ansIdx += currentCol - targetCol;

            memset(ans + ansIdx, '^', currentRow - targetRow);
            ansIdx += currentRow - targetRow;
        }
        /* Need to go up and right, no need to avoid blank space */
        else if (targetCol >= currentCol && targetRow <= currentRow && currentCol != 0)
        {
            memset(ans + ansIdx, '^', currentRow - targetRow);
            ansIdx += currentRow - targetRow;

            memset(ans + ansIdx, '>', targetCol - currentCol);
            ansIdx += targetCol - currentCol;
        }
        /* Need to go up and right, avoiding blank space */
        else if (targetCol >= currentCol && targetRow <= currentRow)
        {
            memset(ans + ansIdx, '>', targetCol - currentCol);
            ansIdx += targetCol - currentCol;

            memset(ans + ansIdx, '^', currentRow - targetRow);
            ansIdx += currentRow - targetRow;
        }
        else
        {
            printf("Missed a case!\n");
        }

        currentRow = targetRow;
        currentCol = targetCol;
        ans[ansIdx++] = 'A';
        ans[ansIdx++] = '\0';

        totalLen += pullbackArrowKeysRecursive(ans, ansIdx - 1, numRobotsLeft - 1);
        ansIdx = 0;
    }

    /* Save this value for future use */
    addSavedVal(target, targetLen, numRobotsLeft, totalLen);

    return totalLen;
}

void addSavedVal(char *target, int targetStrLen, int numRobotsLeft,
                 long long resultLen)
{
    node_t *head = SavedLengths[numRobotsLeft - 1];
    node_t *newNode = malloc(sizeof(node_t));
    /* Copy over the string, including the null terminator */
    memcpy(newNode->keySequence, target, targetStrLen + 1);
    newNode->length = resultLen;
    newNode->next = head;
    SavedLengths[numRobotsLeft - 1] = newNode;
}

long long getSavedVal(char *target, int numRobotsLeft)
{
    long long retVal = -1;
    node_t *current = SavedLengths[numRobotsLeft - 1];
    while (current != NULL && strcmp(current->keySequence, target) != 0)
    {
        current = current->next;
    }

    if (current != NULL)
    {
        retVal = current->length;
    }

    return retVal;
}

char *pullbackArrowKeys(char *target)
{
    int currentRow = ARROW_START_ROW;
    int currentCol = ARROW_START_COL;
    char *ans = malloc(100);
    int ansIdx = 0;
    int targetRow, targetCol;

    for (int i = 0; i < strlen(target); i++)
    {
        getPosOnArrowKeys(target[i], &targetRow, &targetCol);

        /* Need to go down and left, avoiding blank space */
        if (targetRow == 1 && targetCol == 0 && targetCol < currentCol)
        {
            memset(ans + ansIdx, 'v', targetRow - currentRow);
            ansIdx += targetRow - currentRow;

            memset(ans + ansIdx, '<', currentCol - targetCol);
            ansIdx += currentCol - targetCol;
        }
        /* Need to go down and left, but no need to avoid blank space */
        else if (targetCol <= currentCol && targetRow >= currentRow)
        {
            memset(ans + ansIdx, '<', currentCol - targetCol);
            ansIdx += currentCol - targetCol;

            memset(ans + ansIdx, 'v', targetRow - currentRow);
            ansIdx += targetRow - currentRow;
        }
        /* Need to go down and right */
        else if (targetCol >= currentCol && targetRow >= currentRow)
        {
            memset(ans + ansIdx, 'v', targetRow - currentRow);
            ansIdx += targetRow - currentRow;

            memset(ans + ansIdx, '>', targetCol - currentCol);
            ansIdx += targetCol - currentCol;
        }
        /* Need to go up and left */
        else if (targetCol <= currentCol && targetRow <= currentRow)
        {
            memset(ans + ansIdx, '<', currentCol - targetCol);
            ansIdx += currentCol - targetCol;

            memset(ans + ansIdx, '^', currentRow - targetRow);
            ansIdx += currentRow - targetRow;
        }
        /* Need to go up and right */
        else if (targetCol >= currentCol && targetRow <= currentRow && currentCol != 0)
        {
            memset(ans + ansIdx, '^', currentRow - targetRow);
            ansIdx += currentRow - targetRow;

            memset(ans + ansIdx, '>', targetCol - currentCol);
            ansIdx += targetCol - currentCol;
        }
        else if (targetCol >= currentCol && targetRow <= currentRow)
        {
            memset(ans + ansIdx, '>', targetCol - currentCol);
            ansIdx += targetCol - currentCol;

            memset(ans + ansIdx, '^', currentRow - targetRow);
            ansIdx += currentRow - targetRow;
        }
        else
        {
            printf("Missed a case!\n");
        }

        currentRow = targetRow;
        currentCol = targetCol;
        ans[ansIdx++] = 'A';
    }
    ans[ansIdx] = '\0';
    return ans;
}

char *getFirstRobotMoves(char *target, int *currentRow, int *currentCol)
{
    char *ans = malloc(100);
    int ansIdx = 0;
    int targetRow, targetCol, rowDir, colDir;

    for (int i = 0; i < strlen(target); i++)
    {
        getPosOnNumpad(target[i], &targetRow, &targetCol);

        /* Need to go down and right or just down or just right (or nowhere) */
        if (*currentRow <= targetRow && *currentCol <= targetCol)
        {
            memset(ans + ansIdx, 'v', targetRow - *currentRow);
            ansIdx += targetRow - *currentRow;

            memset(ans + ansIdx, '>', targetCol - *currentCol);
            ansIdx += targetCol - *currentCol;
        }
        /* Need to go left and up, but also need to avoid the empty space */
        else if (*currentRow == 3 && targetRow < *currentRow && targetCol == 0)
        {
            memset(ans + ansIdx, '^', *currentRow - targetRow);
            ansIdx += *currentRow - targetRow;

            memset(ans + ansIdx, '<', *currentCol - targetCol);
            ansIdx += *currentCol - targetCol;
        }
        /* Need to go left and up, no need to avoid the empty space */
        else if (targetRow <= *currentRow && targetCol <= *currentCol)
        {
            memset(ans + ansIdx, '<', *currentCol - targetCol);
            ansIdx += *currentCol - targetCol;

            memset(ans + ansIdx, '^', *currentRow - targetRow);
            ansIdx += *currentRow - targetRow;
        }
        /* Need to go left and down */
        else if (targetRow >= *currentRow && targetCol <= *currentCol)
        {
            memset(ans + ansIdx, '<', *currentCol - targetCol);
            ansIdx += *currentCol - targetCol;

            memset(ans + ansIdx, 'v', targetRow - *currentRow);
            ansIdx += targetRow - *currentRow;
        }
        /* Need to go up and right */
        else if (targetRow <= *currentRow && targetCol >= *currentCol)
        {
            memset(ans + ansIdx, '^', *currentRow - targetRow);
            ansIdx += *currentRow - targetRow;

            memset(ans + ansIdx, '>', targetCol - *currentCol);
            ansIdx += targetCol - *currentCol;
        }
        else
        {
            printf("Missed a case!\n");
        }
        *currentRow = targetRow;
        *currentCol = targetCol;

        ans[ansIdx++] = 'A';
    }
    ans[ansIdx] = '\0';
    return ans;
}

void getPosOnNumpad(char target, int *row, int *col)
{
    char rows[10] = {3, 2, 2, 2, 1, 1, 1, 0, 0, 0};
    char cols[10] = {1, 0, 1, 2, 0, 1, 2, 0, 1, 2};

    if (target == 'A')
    {
        *row = 3;
        *col = 2;
    }
    else
    {
        *row = rows[target - '0'];
        *col = cols[target - '0'];
    }
}

void getPosOnArrowKeys(char target, int *row, int *col)
{
    switch(target)
    {
        case 'A':
            *row = 0;
            *col = 2;
            break;
        case '^':
            *row = 0;
            *col = 1;
            break;
        case '<':
            *row = 1;
            *col = 0;
            break;
        case 'v':
            *row = 1;
            *col = 1;
            break;
        case '>':
            *row = 1;
            *col = 2;
            break;
    }
}
