#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MAX_BUFFER_LEN  20000
#define MUL_MIN_CMD_LEN 8          /* length of mul(1,1) */
#define DO_CMD          "do()"
#define DONT_CMD        "don't()"

int parseNumber(char *strStart, char expEndChar, int *numDigits);

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

    /* Read the whole file into one big buffer */
    char buffer[MAX_BUFFER_LEN + 1];
    size_t bufferLen = 0;
    if (filePointer != NULL) {
        bufferLen = fread(buffer, sizeof(char), MAX_BUFFER_LEN, filePointer);
        if ( ferror( filePointer ) != 0 ) {
            printf("Error reading file\n");
        } else {
            buffer[bufferLen] = '\0';
        }

        fclose(filePointer);
    }

    printf("%zu\n", bufferLen);

    int firstNum = 0;
    int secondNum = 0;
    int numDigits = 0;
    int total = 0;
    int totalWithDosAndDonts = 0;
    bool mulEnabled = true;
    for (int i = 0; i < bufferLen; i++)
    {
        if (buffer[i] == 'd')
        {
            if ( (i + strlen(DO_CMD) <= bufferLen) &&
                 (memcmp(buffer + i, DO_CMD, strlen(DO_CMD)) == 0) )
            {
                mulEnabled = true;
            }
            else if ( (i + strlen(DONT_CMD) <= bufferLen) &&
                      (memcmp(buffer + i, DONT_CMD, strlen(DONT_CMD)) == 0) )
            {
                mulEnabled = false;
            }
        }
        else if ( (buffer[i] == 'm') && (i + MUL_MIN_CMD_LEN <= bufferLen) &&
                  (buffer[i+1] == 'u') && (buffer[i+2] == 'l') &&
                  (buffer[i+3] == '(') )
        {
            // printf("in if statement at idx %d\n", i);
            // Start parsing number after the "mul("
            firstNum = parseNumber(buffer + i + 4, ',', &numDigits);
            if (firstNum != -1)
            {
                // Start parsing number after the "mul(X,", where X is a number
                // with numDigits digits (saved in firstNum)
                secondNum = parseNumber(buffer + i + 4 + numDigits + 1, ')', &numDigits);
                if (secondNum != -1)
                {
                    total += (firstNum * secondNum);
                    if (mulEnabled)
                    {
                        totalWithDosAndDonts += (firstNum * secondNum);
                    }
                }
            }
        }
    }

    printf("total is %d\n", total);
    printf("total taking into account do() and don't() is %d\n",
           totalWithDosAndDonts);

}

int parseNumber(char *strStart, char expEndChar, int *numDigits)
{
    int i = 0;
    char numberStr[10];
    *numDigits = 0;
    while ( ('0' <= strStart[i]) && (strStart[i] <= '9') )
    {
        numberStr[i] = strStart[i];
        i++;
    }
    numberStr[i] = '\0';
    if (strStart[i] != expEndChar)
    {
        return -1;
    }
    else
    {
        *numDigits = i;
        return (atoi(numberStr));
    }
}