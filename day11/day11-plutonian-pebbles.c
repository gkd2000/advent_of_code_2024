#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct node
{
    char *val;
    struct node *next;
} node_t;

int numPlaces (long long n);
void iterateList(node_t *curNode);
long long iteratePebble(char *pebble, int recursionLvl);
long long iteratePebbleMemoized(char *pebble, int recursionLvl);

long long totalPebbles[10000][75];

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
    memset (totalPebbles, 0x00, sizeof(totalPebbles));

    node_t *head = NULL;

    /* Read the whole file into a 2D array, preserving structure */
    char *line = NULL;
    size_t lenCap = 0;
    int listLen = 0;
    int currentRow = 0;
    char *currentNumStr;
    listLen = getline(&line, &lenCap, filePointer);
    node_t *newNodePtr = NULL;
    node_t *prevNodePtr = NULL;
    while ((currentNumStr = strsep(&line, " ")) != NULL)
    {
        if (*currentNumStr != '\0')
        {
            /* Allocate space for a new node */
            newNodePtr = malloc(sizeof(node_t));

            /* Link this node to the previous one */
            if (prevNodePtr != NULL)
            {
                prevNodePtr->next = newNodePtr;
            }
            else
            {
                /* We just allocated the first node. Make it the head */
                head = newNodePtr;
            }

            /* Initialize this node */
            newNodePtr->val = strdup(currentNumStr);
            newNodePtr->next = NULL;

            /* Save the newly-created node to link it to the next one */
            prevNodePtr = newNodePtr;
        }
    }

    long long total = 0;
    while (head != NULL)
    {
        total += iteratePebbleMemoized(head->val, 0);
        head = head->next;
    }

    // node_t *curNode = head;
    // for (int i = 0; i < 25; i++)
    // {
    //     iterateList(head);
    //     curNode = head;
    //     while (curNode != NULL)
    //     {
    //         printf("%s ", curNode->val);
    //         curNode = curNode->next;
    //     }
    //     printf("\n");
    // }
    // // iterateList(head);

    // curNode = head;
    // long long count = 0;
    // while (curNode != NULL)
    // {
    //     count++;
    //     // printf("%s ", curNode->val);
    //     curNode = curNode->next;
    // }
    printf("\nnumber of pebbles is %lld\n", total);
}

void iterateList(node_t *curNode)
{
    int len;
    long long valAsInt;
    char *origVal;
    while (curNode != NULL)
    {
        len = strlen(curNode->val);
        // printf("%s with length %d\n", curNode->val, len);
        if (strcmp(curNode->val, "0") == 0)
        {
            (curNode->val)[0] = '1';
        }
        else if (len % 2 == 0)
        {
            /* Split the string into two */
            origVal = curNode->val;
            curNode->val = strndup(origVal, len / 2);
            node_t *newNodePtr = malloc(sizeof(node_t));

            /* Iterate past leading 0s in the second half of the string */
            int newLen = len / 2;
            char *digit = origVal + (len / 2);
            while ( (*digit == '0') && (strlen(digit) > 1) )
            {
                digit++;
                newLen--;
            }
            newNodePtr->val = strndup(digit, newLen);
            newNodePtr->next = curNode->next;
            curNode->next = newNodePtr;
            curNode = newNodePtr;
            free(origVal);
        }
        else
        {
            valAsInt = (long long) atoi(curNode->val);
            free(curNode->val);
            valAsInt *= 2024;
            asprintf(&curNode->val, "%lld", valAsInt);
        }
        curNode = curNode->next;
    }
}

long long iteratePebble(char *pebble, int recursionLvl)
{
    int len = strlen(pebble);
    // long long pebbleNum = atoll(pebble);
    // long long retVal = 0;
    // printf("%s of len %d at level %d\n", pebble, len, recursionLvl);
    if (recursionLvl == 25)
    {
        // printf("%s ", pebble);
        return 1;
    }
    else if (strcmp(pebble, "0") == 0)
    {
        return iteratePebble("1", recursionLvl + 1);
    }
    else if (len % 2 == 0)
    {
        /* Split the string into two */
        int newLen = len / 2;
        char *digit = pebble + (len / 2);
        while ( (*digit == '0') && (strlen(digit) > 1) )
        {
            digit++;
            newLen--;
        }
        char firstHalf[50];
        memcpy(firstHalf, pebble, len / 2);
        firstHalf[(len / 2)] = '\0';

        return iteratePebble(firstHalf, recursionLvl + 1)
                 + iteratePebble(digit, recursionLvl + 1);
    }
    else
    {
        long long valAsInt = (long long) atoll(pebble);
        valAsInt *= 2024;
        char newPebble[50];
        sprintf(newPebble, "%lld", valAsInt);
        return iteratePebble(newPebble, recursionLvl + 1);
    }
}

long long iteratePebbleMemoized(char *pebble, int recursionLvl)
{
    int len = strlen(pebble);
    long long pebbleNum = atoll(pebble);
    long long retVal = 0;
    // printf("%s of len %d at level %d\n", pebble, len, recursionLvl);
    if (recursionLvl == 75)
    {
        // printf("%s ", pebble);
        return 1;
    }
    else if (pebbleNum < 10000 && totalPebbles[pebbleNum][recursionLvl] != 0)
    {
        // printf("returning row %lld col %d, value of %lld\n", pebbleNum, recursionLvl,
            //    totalPebbles[pebbleNum][recursionLvl]);
        return totalPebbles[pebbleNum][recursionLvl];
    }
    else if (strcmp(pebble, "0") == 0)
    {
        retVal = iteratePebbleMemoized("1", recursionLvl + 1);
        if (pebbleNum < 10000)
        {
            totalPebbles[pebbleNum][recursionLvl] = retVal;
        }
        return retVal;
    }
    else if (len % 2 == 0)
    {
        /* Split the string into two */
        int newLen = len / 2;
        char *digit = pebble + (len / 2);
        while ( (*digit == '0') && (strlen(digit) > 1) )
        {
            digit++;
            newLen--;
        }
        char firstHalf[50];
        memcpy(firstHalf, pebble, len / 2);
        firstHalf[(len / 2)] = '\0';

        retVal = iteratePebbleMemoized(firstHalf, recursionLvl + 1)
                 + iteratePebbleMemoized(digit, recursionLvl + 1);
        // printf("\firstHalf is %s, digit is %s\n", firstHalf, digit);
        if (pebbleNum < 10000)
        {
            totalPebbles[pebbleNum][recursionLvl] = retVal;
        }
        return retVal;
    }
    else
    {
        long long valAsInt = (long long) atoll(pebble);
        valAsInt *= 2024;
        char newPebble[50];
        sprintf(newPebble, "%lld", valAsInt);
        retVal = iteratePebbleMemoized(newPebble, recursionLvl + 1);
        if (pebbleNum < 10000)
        {
            totalPebbles[pebbleNum][recursionLvl] = retVal;
        }
        return retVal;
    }
}

/* Given an integer n, return the number of decimal digits it has */
int numPlaces (long long n)
{
    if (n == 0) return 1;
    return floor (log10 (n)) + 1;
}