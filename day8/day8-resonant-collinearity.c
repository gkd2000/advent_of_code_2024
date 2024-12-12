#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#define GRID_DIM 50
#define MAX_DISTINCT_NODES ('z' - '0' + 1)

bool AntinodesFound[GRID_DIM][GRID_DIM];
bool ResonantAntinodesFound[GRID_DIM][GRID_DIM];

/*
 * List of pointers to integers. The ith entry is a list of positions of nodes
 * matching the letter/number i + '0'.
 */
int *ListsOfNodes[MAX_DISTINCT_NODES];
int ListOfLengths[MAX_DISTINCT_NODES];

void processFile (FILE *filePointer);
int getAntinodes(int *nodeList, int len);
int getResonantAntinodes(int *nodeList, int len);
bool isInBounds (int x, int y);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    for (int i = 0; i < MAX_DISTINCT_NODES; i++)
    {
        ListsOfNodes[i] = NULL;
    }

    /* Attempt to open the file */
    char *fileName = argv[1];
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    /* Process the input file */
    processFile (filePointer);
    fclose (filePointer);

    int *list;
    int totalAntinodes = 0;
    int totalResonantAntinodes = 0;
    for (int i = 0; i < MAX_DISTINCT_NODES; i++)
    {
        list = ListsOfNodes[i];
        if (list != NULL)
        {
            printf("list for %c: ", i + '0');
            for (int j = 0; j < ListOfLengths[i]; j++)
            {
                printf("%d ", list[j]);
            }
            printf("\n");
            totalAntinodes += getAntinodes(list, ListOfLengths[i]);
            totalResonantAntinodes += getResonantAntinodes(list, ListOfLengths[i]);
        }
    }
    printf("total antinodes is %d\n", totalAntinodes);
    printf("total resonant antinodes is %d\n", totalResonantAntinodes);
}

void processFile (FILE *filePointer)
{
    char *line = NULL;
    size_t lineCap = 0;
    int len = 0;
    char currentChar;
    int currentLine = 0;

    while ((len = getline(&line, &lineCap, filePointer)) != -1)
    {
        printf("%s", line);
        /* Iterate over the line, excluding the newline character */
        for (int lineIdx = 0; lineIdx < len - 1; lineIdx++)
        {
            printf("%c", line[lineIdx]);
            currentChar = line[lineIdx];
            if ((currentChar != '.') &&
                (ListsOfNodes[currentChar - '0'] == NULL))
            {
                /* No list exists yet for this character. Create one */
                int *newList = malloc(sizeof(int) * 40);
                ListsOfNodes[currentChar - '0'] = newList;
                newList[0] = currentLine;
                newList[1] = lineIdx;
                ListOfLengths[currentChar - '0'] = 2;
            }
            else if (currentChar != '.')
            {
                /* A list already exists for this character. Add to it */
                int *currentList = ListsOfNodes[currentChar - '0'];
                int currentListLen = ListOfLengths[currentChar - '0'];
                currentList[currentListLen] = currentLine;
                currentList[currentListLen + 1] = lineIdx;
                ListOfLengths[currentChar - '0'] = currentListLen + 2;
            }
        }
        currentLine++;
    }
}

/* Given a list of node positions, generate all of the antinodes */
int getAntinodes(int *nodeList, int len)
{
    int run = 0;
    int rise = 0;
    int total = 0;
    int antinodeX = 0;
    int antinodeY = 0;
    /* Iterate over all pairs of positions */
    for (int i = 0; i < len; i+= 2)
    {
        for (int j = i + 2; j < len; j+= 2)
        {
            printf("(%d, %d) and (%d, %d)\n", nodeList[i], nodeList[i+1], nodeList[j], nodeList[j+1]);
            run = nodeList[j] - nodeList[i];
            rise = nodeList[j+1] - nodeList[i+1];
            printf("\trise: %d, run: %d\n", rise, run);
            antinodeX = nodeList[i] - run;
            antinodeY = nodeList[i+1] - rise;
            printf("\t%d, %d\n", antinodeX, antinodeY);
            if ( (isInBounds(antinodeX, antinodeY)) &&
                 (!AntinodesFound[antinodeX][antinodeY]) )
            {
                AntinodesFound[antinodeX][antinodeY] = true;
                total++;
            }

            antinodeX = nodeList[j] + run;
            antinodeY = nodeList[j+1] + rise;
            printf("\t%d, %d\n", antinodeX, antinodeY);
            if ( (isInBounds(antinodeX, antinodeY)) &&
                 (!AntinodesFound[antinodeX][antinodeY]) )
            {
                AntinodesFound[antinodeX][antinodeY] = true;
                total++;
            }
        }
    }
    return total;
}

int getResonantAntinodes(int *nodeList, int len)
{
    int run = 0;
    int rise = 0;
    int total = 0;
    int antinodeX = 0;
    int antinodeY = 0;
    /* Iterate over all pairs of positions */
    for (int i = 0; i < len; i+= 2)
    {
        for (int j = i + 2; j < len; j+= 2)
        {
            printf("(%d, %d) and (%d, %d)\n", nodeList[i], nodeList[i+1], nodeList[j], nodeList[j+1]);
            run = nodeList[j] - nodeList[i];
            rise = nodeList[j+1] - nodeList[i+1];
            printf("\trise: %d, run: %d\n", rise, run);
            antinodeX = nodeList[i] - run;
            antinodeY = nodeList[i+1] - rise;
            while (isInBounds(antinodeX, antinodeY))
            {
                printf("\t%d, %d\n", antinodeX, antinodeY);
                if (!ResonantAntinodesFound[antinodeX][antinodeY])
                {
                    ResonantAntinodesFound[antinodeX][antinodeY] = true;
                    total++;
                }
                antinodeX -= run;
                antinodeY -= rise;
            }

            antinodeX = nodeList[i];
            antinodeY = nodeList[i+1];
            while (isInBounds(antinodeX, antinodeY))
            {
                printf("\t%d, %d\n", antinodeX, antinodeY);
                if (!ResonantAntinodesFound[antinodeX][antinodeY])
                {
                    ResonantAntinodesFound[antinodeX][antinodeY] = true;
                    total++;
                }
                antinodeX += run;
                antinodeY += rise;
            }
        }
    }
    return total;
}

bool isInBounds (int x, int y)
{
    return ( (x >= 0) && (x < GRID_DIM) && (y >= 0) && (y < GRID_DIM) );
}