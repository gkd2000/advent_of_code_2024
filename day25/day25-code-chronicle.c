#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define WIDTH 5
#define NUM_KEYS 300
#define NUM_LOCKS 300

int Keys[NUM_KEYS][WIDTH];
int Locks[NUM_LOCKS][WIDTH];

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

    memset(Keys, 0, sizeof(Keys));
    memset(Locks, 0, sizeof(Locks));

    /* Read the input file */
    char *line = NULL;
    size_t maxLen = 0;
    int lineLen = 0;
    int currentLine = 1;
    int currentKey = 0;
    int currentLock = 0;
    bool isKey;
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1)
    {
        /* Don't count the bottom lines of the keys towards the total */
        if (!isKey || currentLine % 8 != 7)
        {
            if (currentLine % 8 == 1)
            {
                isKey = (memcmp(line, ".....", 5) == 0);
                printf("this is a %s\n", isKey ? "key" : "lock");
            }
            else if (currentLine % 8 != 0)
            {
                for (int i = 0; i < WIDTH; i++)
                {
                    if (isKey)
                    {
                        Keys[currentKey][i] += line[i] == '#' ? 1 : 0;
                        printf("currentKey: %d, i: %d\n", currentKey, i);
                    }
                    else
                    {
                        Locks[currentLock][i] += line[i] == '#' ? 1 : 0;
                        printf("currentLock: %d, i: %d\n", currentLock, i);
                    }
                }
            }
            else if (isKey)
            {
                currentKey++;
            }
            else
            {
                currentLock++;
            }
        }
        currentLine++;
    }

    if (isKey)
    {
        currentKey++;
    }
    else
    {
        currentLock++;
    }

    /* Print out keys and locks */
    for (int i = 0; i < currentKey; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            printf("%d ", Keys[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < currentLock; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            printf("%d ", Locks[i][j]);
        }
        printf("\n");
    }

    /* Iterate through all the keys, checking if they fit with each of the locks */
    bool fitsSoFar = true;
    int totalCombos = 0;
    for (int key = 0; key < currentKey; key++)
    {
        for (int lock = 0; lock < currentLock; lock++)
        {
            fitsSoFar = true;
            for (int col = 0; col < WIDTH && fitsSoFar; col++)
            {
                fitsSoFar = Locks[lock][col] + Keys[key][col] <= 5;
            }

            if (fitsSoFar)
            {
                totalCombos++;
            }
        }
    }

    printf("total combos is %d\n", totalCombos);

}