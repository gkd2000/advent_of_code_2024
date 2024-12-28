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

    int idx = 0;
    int num;
    char *str;
    int total = 0;
    while ((str = strsep(&line, " ")) != NULL)
    {
        if (str[0] != '.')
        {
            num = atoi(str);
            printf("%d * %d\n", idx, num);
            total += idx * num;
        }
        else
        {
            printf("%d * 0\n", idx);
        }
        idx++;
    }

    printf("%d\n", total);
}