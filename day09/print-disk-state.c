#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

int numPlaces (int n);

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

    int id = 0;
    for (int i = 0; i < inputSize; i++)
    {
        if (i % 2 == 0)
        {
            /* Print occupied disk space */
            for (int j = 0; j < line[i] - '0'; j++)
            {
                printf("%d ", id);
            }
            id++;
        }
        else
        {
            /* Print free space */
            for (int j = 0; j < line[i] - '0'; j++)
            {
                printf(". ");
            }
        }
    }
    printf("\n");
}

/* Given an integer n, return the number of decimal digits it has */
int numPlaces (int n)
{
    if (n == 0) return 1;
    return floor (log10 (abs (n))) + 1;
}