#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX_VALUE 100
#define MAX_PAGELIST_LEN 30
#define MAX_DIGITS_PER_NUMBER 4
int RuleMatrix[MAX_VALUE][MAX_VALUE];

void processRules (FILE *filePointer);
int processLists (FILE *filePointer);
bool validateList (int *pageList, int pageListLen);
void reorderList (int *pageList, int pageListLen);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    /* Set the RuleMatrix to 0, just in case */
    memset (RuleMatrix, 0, sizeof(RuleMatrix));

    /* Attempt to open the file */
    char *fileName = argv[1];
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    /*
     * Process the input file
     */
    processRules (filePointer);
    int totalMiddlePagesInitiallyCorrect = processLists (filePointer);

    fclose (filePointer);

    // printf("total of initially correct middle pages is %d\n",
    //        totalMiddlePagesInitiallyCorrect);
}

void processRules (FILE *filePointer)
{
    char *line = NULL;
    size_t len = 0;
    char *firstNumStr = malloc(MAX_DIGITS_PER_NUMBER);
    char *secondNumStr = NULL;
    int firstNum = 0;
    int secondNum  = 0;

    /* Read the file one line at a time until we get to an empty line */
    while ((getline(&line, &len, filePointer) != -1) && (*line != '\n'))
    {
        /* Get the first number before the delimiter */
        firstNumStr = strsep(&line, "|");

        /*
         * Call strsep() to process the delimiter, then stop when we find a
         * non-delimiter character (this is the second number)
         */
        while (*(secondNumStr = (strsep(&line, "|"))) == '\0');

        firstNum = atoi(firstNumStr);
        secondNum = atoi(secondNumStr);

        RuleMatrix[firstNum][secondNum] = 1;
    }

    free(firstNumStr);
}

int processLists (FILE *filePointer)
{
    char *line = NULL;
    size_t len = 0;
    char *currentNumStr;
    int pageList[MAX_PAGELIST_LEN];
    int pageListLen = 0;
    int totalMiddlePagesInitiallyCorrect = 0;
    int totalMiddlePagesCorrected = 0;

    while (getline(&line, &len, filePointer) != -1)
    {
        pageListLen = 0;
        while ((currentNumStr = strsep(&line, ",")) != NULL)
        {
            pageList[pageListLen] = atoi(currentNumStr);
            pageListLen++;
        }
        // for (int i = 0; i < pageListLen; i++)
        // {
        //     printf("%d ", pageList[i]);
        // }
        // printf("\n");

        if (validateList(pageList, pageListLen))
        {
            totalMiddlePagesInitiallyCorrect += pageList[(pageListLen - 1) / 2];
        }
        else
        {
            reorderList(pageList, pageListLen);
            totalMiddlePagesCorrected += pageList[(pageListLen - 1) / 2];

            for (int i = 0; i < pageListLen; i++)
            {
                printf("%d ", pageList[i]);
            }
            printf("\n");
        }
    }
    printf("total of initially correct middle pages is %d\n",
           totalMiddlePagesInitiallyCorrect);
    printf("total middle pages corrected is %d\n", totalMiddlePagesCorrected);

    return totalMiddlePagesInitiallyCorrect;
}

bool validateList (int *pageList, int pageListLen)
{
    bool validSoFar = true;
    for (int *firstNum = pageList; (firstNum < pageList + pageListLen) && validSoFar; firstNum++)
    {
        for (int *laterNum = firstNum + 1; (laterNum < pageList + pageListLen) && validSoFar; laterNum++)
        {
            // printf ("%d %d\n", *firstNum, *laterNum);
            /*
             * Need to make sure that there is no rule that says that
             * pageList[i] should follow pageList[j] and no rule that says that
             * pageList[j] should precede pageList[i]
             * So want to validate that j|i is not a rule.
             */
            if (RuleMatrix[*laterNum][*firstNum] == 1)
            {
                validSoFar = false;
            }
        }
    }

    return validSoFar;
}

void reorderList (int *pageList, int pageListLen)
{
    int temp = 0;
    for (int i = 0; i < pageListLen - 1; i++)
    {
        /* Determine if we need to swap the elements at indices i and i+1 */
        if (RuleMatrix[pageList[i+1]][pageList[i]] == 1)
        {
            /* pageList[i+1] should preced pageList[i]. Swap them */
            temp = pageList[i+1];
            pageList[i+1] = pageList[i];
            pageList[i] = temp;
            
            /*
             * Iterate back down through the list to see if we need to move the
             * ith element (which was formerly the i+1st element) down any further
             */
            int j = i;
            while (j > 0 && RuleMatrix[pageList[j]][pageList[j-1]] == 1)
            {
                temp = pageList[j];
                pageList[j] = pageList[j-1];
                pageList[j-1] = temp;
                j--;
            }
            // for (int j = i; j > 0; j--)
            // {
            //     if (RuleMatrix[pageList[j]][pageList[j-1]] == 1)
            //     {
            //         temp = pageList[j];
            //         pageList[j] = pageList[j-1];
            //         pageList[j-1] = temp;
            //     }
            // }
        }
    }

    for (int i = 0; i < pageListLen; i++)
    {
        printf("%d ", pageList[i]);
    }
    printf("\n");
}
