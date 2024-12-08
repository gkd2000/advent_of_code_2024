#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_VALUE 100
#define MAX_OPERANDS 30

void processLine (FILE *filePointer);
bool simpleComboExists(long long target, long long *operandsList,
                       int operandsListLen);
int numPlaces (int n);
void getTernaryArr(unsigned int bitStrDecimal, int *opBitStrTernary,
                   int numTernaryDigits);
bool concatComboExists(long long target, long long *operandsList,
                       int operandsListLen);
long long concatInts (long long first, long long second);

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

    /* Process the input file */
    processLine (filePointer);

    fclose (filePointer);
}


void processLine (FILE *filePointer)
{
    char *line = NULL;
    size_t len = 0;
    char *currentNumStr;
    long long operandsList[MAX_OPERANDS];
    long long target;
    int operandsListLen = 0;
    long long total = 0;
    long long totalFromConcat = 0;

    while (getline(&line, &len, filePointer) != -1)
    {
        /*
         * Read in the current line, saving the target and the operands as 
         * long longs
         */
        operandsListLen = 0;
        currentNumStr = strsep(&line, ":");
        target = atoll(currentNumStr);

        while ((currentNumStr = strsep(&line, " ")) != NULL)
        {
            if(isdigit(currentNumStr[0])){
                operandsList[operandsListLen] = atoll(currentNumStr);
                operandsListLen++;
            }
        }

        if(simpleComboExists(target, operandsList, operandsListLen))
        {
            total += target;
        }
        else if (concatComboExists(target, operandsList, operandsListLen))
        {
            totalFromConcat += target;
        }
    }

    printf("total is %lld\n", total);
    printf("totalFromConcat is %lld\n", totalFromConcat);
    printf("grand total is %lld\n", total + totalFromConcat);
}

/* Given an integer n, return the number of decimal digits it has */
int numPlaces (int n)
{
    if (n == 0) return 1;
    return floor (log10 (abs (n))) + 1;
}

/*
 * Associate each number from 0 to 2^(operandsListLen-1) with a sequence
 * of either + of * of length operandsListLen - 1. In this way, compute all
 * possible combinations of elements of operandsList by + and *, checking
 * whether any of them matches target. Returns true if a match is found and
 * false otherwise.
 */
bool simpleComboExists(long long target, long long *operandsList,
                       int operandsListLen)
{
    // Number of lists of length operandsListLen - 1 whose entries are either + or *
    unsigned int numCombos = (unsigned int) pow(2, operandsListLen - 1);
    long long totalSoFar = operandsList[0];
    unsigned int mask = 0;
    bool foundCombo = false;
    /* Iterate over all of the combinations of + and * */
    for (unsigned int operatorBitStr = 0;
         operatorBitStr < numCombos && !foundCombo;
         operatorBitStr++)
    {
        /* For this combination, compute the total */
        mask = 1;
        totalSoFar = operandsList[0];

        for (int operandIdx = 1;
             (operandIdx < operandsListLen) && (totalSoFar <= target); 
             operandIdx++)
        {
            if ((operatorBitStr & mask) != 0)
            {
                /*
                 * operandIdx'th bit (1-indexed, starting from the right)
                 * is set. Do addition.
                 */
                totalSoFar += operandsList[operandIdx];
            }
            else
            {
                /* Do multiplication */
                totalSoFar *= operandsList[operandIdx];
            }
            mask = mask << 1;
        }

        if (totalSoFar == target)
        {
            foundCombo = true;
        }
    }

    return foundCombo;
}

/*
 * Given a list of numbers and a target number, determine whether the numbers
 * in the list can be combined to give the target, using addition, multiplication
 * and concatenation. For a list of numbers of length n, there are n-1 positions
 * in which to put operators, and there are 3 choices per operator. Thus, there
 * are 3^(operandsListLen-1) combinations. Use a ternary representation to keep
 * track of what operator to put in each position.
 */
bool concatComboExists(long long target, long long *operandsList,
                       int operandsListLen)
{
    unsigned int numCombos = (unsigned int) pow(3, operandsListLen - 1);
    long long totalSoFar;
    bool foundCombo = false;
    /*
     * Array where each entry is 0, 1, or 2. Read left to right, it contains the
     * representation of the current combination in ternary.
     */
    int *opBitStrTernary = malloc(operandsListLen - 1);

    /* Iterate over all possible combinations */
    for (unsigned int operatorBitStr = 0;
         operatorBitStr < numCombos && !foundCombo;
         operatorBitStr++)
    {
        /* Get a ternary representation of the current combination */
        getTernaryArr(operatorBitStr, opBitStrTernary, operandsListLen - 1);

        /* Combine the operands as directed by the ternary string */
        totalSoFar = operandsList[0];
        for (int operandIdx = 1;
             (operandIdx < operandsListLen) && (totalSoFar <= target);
             operandIdx++)
        {
            switch (opBitStrTernary[operandIdx - 1])
            {
            case 0:
                /* Add */
                totalSoFar += operandsList[operandIdx];
                break;
            case 1:
                /* Multiply */
                totalSoFar *= operandsList[operandIdx];
                break;
            case 2:
                /* Concatenate */
                totalSoFar = concatInts(totalSoFar, operandsList[operandIdx]);
                break;
            default:
                break;
            }
        }

        if (totalSoFar == target)
        {
            foundCombo = true;
        }
    }
    free(opBitStrTernary);

    return foundCombo;
}

/* Concatenate two integers */
long long concatInts (long long first, long long second)
{
    /* "Shift" first over by the number of digits in second to make room for second */
    first *= (long long) (pow(10, numPlaces(second)));
    return first + second;
}

/*
 * Given an integer, an array of integers, and a requested number of digits, fill
 * the array with the ternary representation of the integer, up to the requested
 * number of digits. Ternary representation will have the least significant bit
 * on the right.
 */
void getTernaryArr(unsigned int bitStrDecimal, int *opBitStrTernary,
                   int numTernaryDigits)
{
    int currentDigit = numTernaryDigits - 1;
    for (int currentDigit = numTernaryDigits - 1; currentDigit >= 0; currentDigit--)
    {
        opBitStrTernary[currentDigit] = bitStrDecimal % 3;
        bitStrDecimal /= 3;
    }
}