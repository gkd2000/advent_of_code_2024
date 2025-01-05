#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define NUM_LETTERS 26
#define NUM_COMPUTERS NUM_LETTERS * NUM_LETTERS

bool AdjMatrix[NUM_COMPUTERS][NUM_COMPUTERS];
void getHostIdFromIdx(int idx, char *hostId);
int getHostIdx(char *hostName);
bool startsWithT(int idx);
void recordClique(int row, int col, int candidate, int *cliques, int idx);
void orderVertices(int row, int col, int candidate, int *first, int *middle, int *last);

int main (int argc, char **argv)
{
    /* Validate argument */
    // if (argc < 3)
    // {
    //     printf("Please provide an input file\n");
    //     return 0;
    // }

    /* Attempt to open the file */
    // char *fileName = argv[1];
    FILE *filePointer = fopen("input.txt", "r");
    if (filePointer == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }
    memset (AdjMatrix, 0x00, sizeof(AdjMatrix));

    // int cliqueSize = atoi(argv[2]);
    int cliqueSize = 14;
    printf("clique size %d\n", cliqueSize);

    /* Read the file and construct the adjacency matrix */
    char *line = NULL;
    size_t lenCap = 0;
    int listLen = 0;
    int currentRow = 0;
    char *currentNumStr;
    char* firstHost;
    int firstHostIdx, secondHostIdx;
    while(getline(&line, &lenCap, filePointer) != -1)
    {
        firstHost = strsep(&line, "-");
        firstHostIdx = getHostIdx(firstHost);
        secondHostIdx = getHostIdx(line);
        AdjMatrix[firstHostIdx][secondHostIdx] = true;
        AdjMatrix[secondHostIdx][firstHostIdx] = true;
    }

    int total = 0;
    /*
     * Iterate over all the edges, but only need to do the upper half of the
     * matrix (above the main diagonal), since the graph is not directed and so
     * the matrix is symmetric.
     */
    char host1[3];
    char host2[3];
    char host3[3];
    // int *cliques = malloc(1400 * cliqueSize * sizeof(int));
    int first, middle, last;
    bool alreadyFound;
    int *candidateClique = malloc(cliqueSize * sizeof(int));
    if (candidateClique == NULL)
    {
        printf("malloc failed\n");
        return 0;
    }
    int verticesFound = 0;
    bool formsCliqueSoFar = true;
    bool cliqueOfDesiredSizeExists = false;
    for (int row = 0; row < NUM_COMPUTERS && !cliqueOfDesiredSizeExists; row++)
    {
        for (int col = row + 1; col < NUM_COMPUTERS && !cliqueOfDesiredSizeExists; col++)
        {
            if (AdjMatrix[row][col])
            {
                /*
                 * row-col is an edge. We want to see if there is a clique with
                 * cliqueSize vertices containing these two vertices
                 */
                printf("iterating over vertices\n");
                candidateClique[0] = row;
                candidateClique[1] = col;
                verticesFound = 2;
                for (int candidate = 0;
                     candidate < NUM_COMPUTERS && !cliqueOfDesiredSizeExists;
                     candidate++)
                {
                    formsCliqueSoFar = true;
                    for (int i = 0; i < verticesFound && formsCliqueSoFar; i++)
                    {
                        if(AdjMatrix[candidateClique[i]][candidate] == false)
                        {
                            formsCliqueSoFar = false;
                        }
                    }
                    if (formsCliqueSoFar)
                    {
                        /* Found a vertex adjacent to all previous vertices */
                        printf("found a new vertex, verticesFound is %d\n", verticesFound);
                        candidateClique[verticesFound] = candidate;
                        verticesFound++;
                        if (verticesFound == cliqueSize)
                        {
                            cliqueOfDesiredSizeExists = true;
                        }
                    }
                }
            }
        }
    }

    char hostName[3];
    if (cliqueOfDesiredSizeExists)
    {
        for (int i = 0; i < cliqueSize; i++)
        {
            getHostIdFromIdx(candidateClique[i], hostName);
            printf("%s ", hostName);
        }
        printf("\n");
    }
    else
    {
        printf("did not find clique\n");
    }

    free(candidateClique);
}

void orderVertices(int row, int col, int candidate, int *first, int *middle, int *last)
{
    if (row < col && col < candidate)
    {
        *first = row;
        *middle = col;
        *last = candidate;
    }
    else if (row < candidate && candidate < col)
    {
        *first = row;
        *middle = candidate;
        *last = col;
    }
    else if (candidate < row && row < col)
    {
        *first = candidate;
        *middle = row;
        *last = col;
    }

    else if (candidate < col && col < row)
    {
        *first = candidate;
        *middle = col;
        *last = row;
    }
    else if (col < row && row < candidate)
    {
        *first = col;
        *middle = row;
        *last = candidate;
    }
    else if (col < candidate && candidate < row)
    {
        *first = col;
        *middle = candidate;
        *last = row;
    }
    else
    {
        printf("Error: missed a case\n");
    }
}

void recordClique(int row, int col, int candidate, int *cliques, int idx)
{
    if (row < col && col < candidate)
    {
        cliques[idx] = row;
        cliques[idx + 1] = col;
        cliques[idx + 2] = candidate;
    }
    else if (row < candidate && candidate < col)
    {
        cliques[idx] = row;
        cliques[idx + 1] = candidate;
        cliques[idx + 2] = col;
    }
    else if (candidate < row && row < col)
    {
        cliques[idx] = candidate;
        cliques[idx + 1] = row;
        cliques[idx + 2] = col;
    }

    else if (candidate < col && col < row)
    {
        cliques[idx] = candidate;
        cliques[idx + 1] = col;
        cliques[idx + 2] = row;
    }
    else if (col < row && row < candidate)
    {
        cliques[idx] = col;
        cliques[idx + 1] = row;
        cliques[idx + 2] = candidate;
    }
    else if (col < candidate && candidate < row)
    {
        cliques[idx] = col;
        cliques[idx + 1] = candidate;
        cliques[idx + 2] = row;
    }
    else
    {
        printf("Error: missed a case\n");
    }
}

/*
 * Given a host name of length 2, get the corresponding row/column in the
 * adjacency matrix
 */
int getHostIdx(char *hostName)
{
    return ((hostName[0] - 'a') * NUM_LETTERS) + (hostName[1] - 'a');
}

void getHostIdFromIdx(int idx, char *hostId)
{
    hostId[2] = '\0';
    hostId[1] = (idx % NUM_LETTERS) + 'a';
    hostId[0] = (idx / NUM_LETTERS) + 'a';
}

/*
 * Given a row or column in the adjacency matrix, return whether the
 * corresponding host name starts with T.
 */
bool startsWithT(int idx)
{
    return (idx / NUM_LETTERS) == 't' - 'a';
}