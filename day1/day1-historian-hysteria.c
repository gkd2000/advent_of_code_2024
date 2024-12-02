#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "merge-sort.h"

#define MAX_DIGITS_PER_NUMBER 10
#define LIST_LENGTH 1000

int processInputFile (FILE *filePointer, int *firstList, int *secondList);
int getDistance (int *firstList, int *secondList, int listLen);
int getSimilarityScore (int *firstList, int *secondList, int listLen);

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

    /*
     * Process the input file, storing the first and second columns in
     * firstList and secondList, respectively
     */
    int firstList[LIST_LENGTH];
    int secondList[LIST_LENGTH];
    int listLen = processInputFile (filePointer, firstList, secondList);

    /* Get distance between lists (part 1) */
    int distance = getDistance (firstList, secondList, listLen);
    printf("Total distance: %d\n", distance);

    /* Get similarity score (part 2) */
    int similarityScore = getSimilarityScore (firstList, secondList, listLen);
    printf("Similarity score is: %d\n", similarityScore);
}

/*
 * Given a file pointer which points to a file containing two space-separated
 * numbers per line, put the first number on each line into firstList and the
 * second into secondList.
 */
int processInputFile (FILE *filePointer, int *firstList, int *secondList)
{
    char *line = NULL;
    size_t len = 0;
    char *firstNumStr = malloc(MAX_DIGITS_PER_NUMBER);
    char *secondNumStr = NULL;
    int firstNum = 0;
    int secondNum  = 0;
    int listLen = 0;

    /* Read the file one line at a time */
    while (getline(&line, &len, filePointer) != -1)
    {
        /* Get the first number before any spaces */
        firstNumStr = strsep(&line, " ");

        /*
         * Call strsep() to process all the spaces, then stop when we find a
         * non-space character (this is the second number)
         */
        while (*(secondNumStr = (strsep(&line, " "))) == '\0');

        firstNum = atoi(firstNumStr);
        secondNum = atoi(secondNumStr);

        firstList[listLen] = firstNum;
        secondList[listLen] = secondNum;
        listLen++;
    }

    free(firstNumStr);

    return listLen;
}

/*
 * Given two lists of integers (of the same length), sort them each and then
 * find the sum of the distances between corresponding entries in the lists.
 */
int getDistance (int *firstList, int *secondList, int listLen)
{
    /* Sort each list */
    MergeSort(firstList, listLen);
    MergeSort(secondList, listLen);

    int distance = 0;
    for (int i = 0; i < listLen; i++)
    {
        distance += abs(firstList[i] - secondList[i]);
    }

    return distance;
}

/*
 * Given two sorted lists of the same length, determine the "similarity score"
 * between the two lists. Each time a number appears in the first list, multiply
 * that number by the number of times it appears in the second list. Add all of
 * these products to obtain the similarity score.
 */
int getSimilarityScore (int *firstList, int *secondList, int listLen)
{
    /*
     * Get the maximum element in the second list, and make a new array with
     * that many elements. secondListCounts[i] is the number of times that the
     * value i appears in secondList.
     */
    int maxVal = secondList[listLen - 1];
    int *secondListCounts = malloc(maxVal * sizeof(int));
    for (int i = 0; i < listLen; i++)
    {
        secondListCounts[secondList[i]]++;
    }

    int similarityScore = 0;
    for (int i = 0; i < listLen; i++)
    {
        /*
         * If firstList[i] is greater than maxVal, then we know that firstList[i]
         * does not appear in secondList. No need to increment similarityScore.
         */
        if (firstList[i] <= maxVal)
        {
            similarityScore += firstList[i] * secondListCounts[firstList[i]];
        }
    }
    free(secondListCounts);

    return similarityScore;
}