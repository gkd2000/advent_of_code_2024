#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define NUM_TOWELS 447
#define NUM_LENGTHS 65
#define NUM_LETTERS 26

/*
 * The i'th row of Towels is the possible combos we've seen of length i. To
 * begin, the i'th row just contains the input towels of length i.
 */
char *Towels[NUM_LENGTHS][NUM_TOWELS];
char *ImpossibleTowels[NUM_LENGTHS][NUM_TOWELS];
char *ComboTowels[NUM_LENGTHS][NUM_TOWELS];
long long NumCombos[NUM_LENGTHS][NUM_TOWELS];
/* NumCombosIdxs[i] is the number of entries in the i'th row of NumCombos. */
int NumCombosIdxs[NUM_LENGTHS];

/* NumTowels[i] is the number of entries in the i'th row of Towels. */
int NumTowels[NUM_LENGTHS];
int NumImpossibleTowels[NUM_LENGTHS];

bool TowelsLen1[NUM_LETTERS];
bool TowelsLen2[NUM_LETTERS][NUM_LETTERS];

bool comboPossible(char *target, int len);
bool singleTowelExists(char *target, int len);
long long getNumCombosKernel(char *target, int startIdx, int len, int origLen);
long long getNumCombos(char *target, int len);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    memset (Towels, '\0', sizeof(Towels));
    memset (NumTowels, 0, sizeof(NumTowels));
    for (int i = 0; i < NUM_LENGTHS; i++)
    {
        for (int j = 0; j < NUM_TOWELS; j++)
        {
            NumCombos[i][j] = -1;
        }
    }

    /* Attempt to open the file */
    char *fileName = argv[1];
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    /* Get the towels */
    char *line = NULL;
    size_t lenCap = 0;
    char *currentTowel;
    char *currentTowelDup;
    int numTowels = 0;
    int towelLen = 0;
    if (getline(&line, &lenCap, filePointer) != -1)
    {
        while ((currentTowel = strsep(&line, " ")) != NULL)
        {
            /* Towels will contain a comma at the end */
            towelLen = strlen(currentTowel) - 1;
            if (towelLen == 1)
            {
                TowelsLen1[(*currentTowel) - 'a'] = true;
            }
            else if (towelLen == 2)
            {
                printf("%c %c\n", *currentTowel, (*(currentTowel + 1)));
                TowelsLen2[(*currentTowel) - 'a'][(*(currentTowel + 1)) - 'a'] = true;
            }
            currentTowelDup = strndup(currentTowel, towelLen);
            Towels[towelLen][NumTowels[towelLen]] = currentTowelDup;
            NumTowels[towelLen] += 1;
        }
    }
    else
    {
        printf("Unable to get list of towels\n");
        return 0;
    }

    int len = 0;
    int totalPossible = 0;
    long long possibleArrangements = 0;
    long long combosPossible = 0;
    /* Process the rest of the file (patterns to make) line by line */
    while ((len = getline(&line, &lenCap, filePointer)) != -1)
    {
        /* Get rid of the newline */
        line[len - 1] = '\0';

        combosPossible = getNumCombos(line, len - 1);
        possibleArrangements += combosPossible;
        if (combosPossible != 0)
        {
            totalPossible++;
        }
        printf("%lld (%s)\n", combosPossible, line);
    }

    /* Clean up */
    fclose (filePointer);

    for (int i = 0; i < NUM_LENGTHS; i++)
    {
        for (int j = 0; j < NumTowels[i]; j++)
        {
            free(Towels[i][j]);
        }
    }

    printf("total possible designs is %d, total arrangements is %lld\n",
           totalPossible, possibleArrangements);
}

/*
 * Calls the recursive method getNumCombosKernel() on each of the prefixes of
 * the target (substrings which contain the first letter)
 */
long long getNumCombos(char *target, int len)
{
    long long total = 0;
    for (int i = 1; i < len; i++)
    {
        total += getNumCombosKernel(target, 0, len - i, len);
    }

    return total;
}

/*
 * Determine the number of ways to spell the first len characters of the string
 * target, starting at startIdx.
 * Let S = s_0s_1...s_len be the first len characters of target, starting at
 * startIdx. Then construct a tree with S as the root. Each node has for
 * children all substrings of target which begin with s_{len+1}. Following a
 * single branch from root to leaf will therefore spell target, starting at
 * startIdx.
 * This method does a depth first search on the tree described above. The total
 * associated with each node is the number of leaves this node has as a
 * descendent which are reachable by only going through nodes which correspond
 * to towels provided by the onsen. Therefore, the total associated with the
 * root is the number of ways to spell target with the towels provided, starting
 * at startIdx.
 */
long long getNumCombosKernel(char *target, int startIdx, int len, int origLen)
{
    long long newTotal = 0;

    /* Base case: node that we're at exists and finishes the target string */
    if (singleTowelExists(target + startIdx, len) && (startIdx + len == origLen))
    {
        return 1;
    }

    /*
     * If the node we're at can be made using a single provided towel, then
     * continue down the tree. Otherwise, stop exploring this branch.
     */
    if (singleTowelExists(target + startIdx, len))
    {
        /*
         * Before recursing, check if we have already saved a value for
         * everything in the target string that follows the single towel we are
         * at.
         */
        bool recursionNeeded = true;
        for (int comboIdx = 0;
             comboIdx < NumCombosIdxs[origLen - startIdx - len]; comboIdx++)
        {
            if (strcmp(ComboTowels[origLen - startIdx - len][comboIdx],
                       target + startIdx + len) == 0)
            {
                /* We have already computed this value! */
                newTotal = NumCombos[origLen - startIdx - len][comboIdx];
                recursionNeeded = false;
            }
        }

        /*
         * If no memoized data was found, compute the number of combinations for
         * all nodes that are children of this one. Their total is the number of
         * combinations for this node.
         */
        if (recursionNeeded)
        {
            int lenRemainingStr = origLen - startIdx - len;
            for (int i = lenRemainingStr; i > 0; i--)
            {
                newTotal += getNumCombosKernel(target, startIdx + len, i,
                                               origLen);
            }
            /*
             * newTotal now stores the number of ways to create the substring at
             * target + startIdx + len (everything after the chunk we're looking at).
             * Save that now.
             */
            char *newComboTowel = strdup(target + startIdx + len);
            ComboTowels[lenRemainingStr][NumCombosIdxs[lenRemainingStr]] = newComboTowel;
            NumCombos[lenRemainingStr][NumCombosIdxs[lenRemainingStr]] = newTotal;
            NumCombosIdxs[lenRemainingStr] += 1;

            /* Print an error and exit if we've filled up our matrix in one dimension */
            if (NumCombosIdxs[lenRemainingStr] == NUM_TOWELS ||
                lenRemainingStr >= NUM_LENGTHS)
            {
                printf("\n\nBAD\n\n");
                exit(0);
            }
        }
    }

    return newTotal;
}

/*
 * This function is no longer called anywhere, but it was my original solution
 * for part 1.
 */
bool comboPossible(char *target, int len)
{
    /* See if we've already encountered this subproblem */
    if (len == 1)
    {
        if (TowelsLen1[*target - 'a'])
        {
            return true;
        }
    }
    else if (len == 2)
    {
        if (TowelsLen2[*target - 'a'][(*(target + 1)) - 'a'])
        {
            return true;
        }
    }

    for (int i = 0; i < NumTowels[len]; i++)
    {
        if (strncmp(target, Towels[len][i], len) == 0 && strlen(Towels[len][i]) == len)
        {
            return true;
        }
    }

    for (int i = 0; i < NumImpossibleTowels[len]; i++)
    {
        if (strncmp(target, ImpossibleTowels[len][i], len) == 0 && strlen(ImpossibleTowels[len][i]) == len)
        {
            return false;
        }
    }

    /* We haven't encountered this subproblem. Do recursion */
    bool isPossible = false;
    for (int i = 1; i < len && !isPossible; i++)
    {
        isPossible = comboPossible(target, i) && comboPossible (target + i, len - i);
    }

    /* Store the result for future iterations */
    if (isPossible)
    {
        if (len == 2)
        {
            TowelsLen2[(*target) - 'a'][(*(target + 1)) - 'a'] = true;
        }
        else
        {
            char *newTowel = strndup(target, len);
            Towels[len][NumTowels[len]] = newTowel;
            NumTowels[len] += 1;
        }
    }
    else
    {
        char *newTowel = strndup(target, len);
        ImpossibleTowels[len][NumImpossibleTowels[len]] = newTowel;
        NumImpossibleTowels[len] += 1;
    }

    return isPossible;
}

bool singleTowelExists(char *target, int len)
{
    bool retVal = false;
    for (int i = 0; i < NumTowels[len]; i++)
    {
        if (strncmp(target, Towels[len][i], len) == 0 &&
            strlen(Towels[len][i]) == len)
        {
            retVal = true;
        }
    }
    return retVal;
}
