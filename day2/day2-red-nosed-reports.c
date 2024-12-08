#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define NUM_LINES_INPUT 1000
#define MAX_REPORT_LENGTH 15

bool isLineSafe (int line[MAX_REPORT_LENGTH])
{
    bool isSafeSoFar = true;
    bool isAscending = true;
    // dampeningUsed = false;
    // isTolerableSoFar = true;
    /* Establish whether the sequence is ascending or descending to start */
    int currentInt = line[1];
    int previousInt = line[0];
    if (currentInt < previousInt)
    {
        isAscending = false;
    }
    else if (currentInt > previousInt)
    {
        isAscending = true;
    }
    else
    {
        isSafeSoFar = false;
    }

    // printf("%d %d ", previousInt, currentInt);
    // previousInt = currentInt;
    int j = 1;
    do {
        // printf("current: %d, previous: %d\n", currentInt, previousInt);
        // printf("%d ", currentInt);
        if ( ((isAscending) && (currentInt <= previousInt)) ||
                ((!isAscending) && (currentInt >= previousInt)) ||
                (abs(currentInt - previousInt) > 3) )
        {
            isSafeSoFar = false;
        }
        j++;
        previousInt = currentInt;
    } while (((currentInt = line[j]) != -1) && isSafeSoFar);
    
    return isSafeSoFar;
    // printf("\n");
}

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

    char *line = NULL;
    size_t len = 0;
    int reportList[NUM_LINES_INPUT][MAX_REPORT_LENGTH];
    char *currentNumStr;
    int currentLine = 0;
    int currentEntry = 0;
    while (getline(&line, &len, filePointer) != -1)
    {
        currentEntry = 0;
        while ((currentNumStr = strsep(&line, " ")) != NULL)
        {
            // printf("currentNumStr is %s\n", currentNumStr);
            if (*currentNumStr != '\0')
            {
                // printf("atoi gives %d, index %d, %d\n", atoi(currentNumStr), currentLine, currentEntry);
                reportList[currentLine][currentEntry] = atoi(currentNumStr);
                currentEntry++;
            }
        }
        reportList[currentLine][currentEntry] = -1;
        currentLine++;
    }

    int j = 0;
    int currentInt = 0;
    int previousInt = 0;
    int safeCounts = 0;
    bool isAscending = true;
    bool isSafeSoFar = true;
    int distance = 0;
    int nextInt = 0;
    int tolerableCounts = 0;
    int subset[15];
    for (int i = 0; i < currentLine; i++)
    {
        if (isLineSafe(reportList[i]))
        {
            safeCounts++;
        }

        j = 0;
        int k;
        while (reportList[i][j] != -1)
        {
            for (k = 0; reportList[i][k] != -1; k++)
            {
                if (k < j)
                {
                    subset[k] = reportList[i][k];
                }
                else if (k > j)
                {
                    subset[k-1] = reportList[i][k];
                }
            }
            subset[k-1] = -1;
            if (isLineSafe(subset))
            {
                tolerableCounts++;
                break;
            }
            j++;
        }

        // isSafeSoFar = true;
        // dampeningUsed = false;
        // isTolerableSoFar = true;
        // /* Establish whether the sequence is ascending or descending to start */
        // currentInt = reportList[i][1];
        // previousInt = reportList[i][0];
        // if (currentInt < previousInt)
        // {
        //     isAscending = false;
        // }
        // else if (currentInt > previousInt)
        // {
        //     isAscending = true;
        // }
        // else
        // {
        //     isSafeSoFar = false;
        // }

        // // printf("%d %d ", previousInt, currentInt);
        // // previousInt = currentInt;
        // j = 1;
        // do {
        //     // printf("current: %d, previous: %d\n", currentInt, previousInt);
        //     // printf("%d ", currentInt);
        //     if ( ((isAscending) && (currentInt <= previousInt)) ||
        //          ((!isAscending) && (currentInt >= previousInt)) ||
        //          (abs(currentInt - previousInt) > 3) )
        //     {
        //         isSafeSoFar = false;
        //     }
        //     j++;
        //     previousInt = currentInt;
        // } while (((currentInt = reportList[i][j]) != -1) && isSafeSoFar);
        // if (isSafeSoFar)
        // {
        //     safeCounts++;
        // }
        // printf("\n");
    }

    printf("Number of safe results: %d\n", safeCounts);
    printf("Number of tolerable results: %d\n", tolerableCounts);

    // for (int i = 0; i < currentLine; i++)
    // {
    //     for (int j = 0; j < MAX_REPORT_LENGTH; j++)
    //     {
    //         printf("%d ", reportList[i][j]);
    //     }
    //     printf ("\n");
    // }

}