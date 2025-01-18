#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define NUM_BUYERS 1798
#define NUM_ITERATIONS 2000
#define SEQUENCE_LEN 4
#define NUM_POSSIBLE_SEQUENCES 19 * 19 * 19 * 19

long long SecretSeeds[NUM_BUYERS];
char Differences[NUM_ITERATIONS + 1];
int SequencesChecked[NUM_ITERATIONS - SEQUENCE_LEN][SEQUENCE_LEN];
int NumSequencesChecked = 0;
char CurrentDifferences[SEQUENCE_LEN];
int SequenceTotals[NUM_POSSIBLE_SEQUENCES][SEQUENCE_LEN + 2];
int SequenceTotalsLen = 0;

/*
 * Maintain a matrix of all the sequences we've seen and how much we could get
 * for them so far. It has a lot of rows rows, but it only needs 6 columns.
 * The first 4 columns of each row contain the sequence. The fifth column
 * contains the total so far if we do use that sequence. The sixth column
 * contains the buyer number at which we last saw this sequence. This is to
 * avoid double counting a sequence if we have seen it before for a particular
 * buyer.
 */

long long evolveNum(long long secretNum);
bool seenSequenceBefore(int idx);
int getProfit(int buyerIdx, int seqStart);
int countDifference (int buyerNum, int price);

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

    memset(SequenceTotals, 0, sizeof(SequenceTotals));

    /* Read the input file and process buyers one at a time */
    char *line = NULL;
    size_t maxLen = 0;
    int lineLen = 0;
    // the pruning number is 2^24, so pruning is doing a bitwise and with
    // 111111111111111111111111 in binary, which is
    // 0xFFFFFF in hex
    long long secretNum = 0;
    long long totalPart1 = 0;
    int currentLine = 0;
    int prevPrice = 0;
    int bestSoFar = 0;
    int newTotal = 0;
    int buyer = 0;
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1)
    {
        printf("%d\n", buyer);
        secretNum = atoll(line);
        prevPrice = secretNum % 10;
        for (int i = 0; i < SEQUENCE_LEN; i++)
        {
            secretNum = evolveNum(secretNum);
            CurrentDifferences[i] = (secretNum % 10) - prevPrice;
            prevPrice = secretNum % 10;
        }
        for (int i = SEQUENCE_LEN; i < NUM_ITERATIONS; i++)
        {
            newTotal = countDifference(buyer, prevPrice);
            if (newTotal > bestSoFar)
            {
                bestSoFar = newTotal;
            }
            secretNum = evolveNum(secretNum);
            CurrentDifferences[0] = CurrentDifferences[1];
            CurrentDifferences[1] = CurrentDifferences[2];
            CurrentDifferences[2] = CurrentDifferences[3];
            CurrentDifferences[3] = (secretNum % 10) - prevPrice;
            prevPrice = secretNum % 10;
        }
        totalPart1 += secretNum;
        buyer++;
    }

    printf("total for part 1 is %lld, total for part 2 is %d\n", totalPart1, bestSoFar);
}

int countDifference (int buyerNum, int price)
{
    bool seqFound = false;
    int retVal = price;
    for (int i = 0; i < SequenceTotalsLen && !seqFound; i++)
    {
        if (CurrentDifferences[0] == SequenceTotals[i][0] &&
            CurrentDifferences[1] == SequenceTotals[i][1] &&
            CurrentDifferences[2] == SequenceTotals[i][2] &&
            CurrentDifferences[3] == SequenceTotals[i][3] &&
            SequenceTotals[i][5] != buyerNum)
        {
            SequenceTotals[i][4] += price;
            SequenceTotals[i][5] = buyerNum;
            retVal = SequenceTotals[i][4];
            seqFound = true;
        }
    }

    /* Need to add a new entry to SequenceTotals */
    if (!seqFound)
    {
        SequenceTotals[SequenceTotalsLen][0] = CurrentDifferences[0];
        SequenceTotals[SequenceTotalsLen][1] = CurrentDifferences[1];
        SequenceTotals[SequenceTotalsLen][2] = CurrentDifferences[2];
        SequenceTotals[SequenceTotalsLen][3] = CurrentDifferences[3];
        SequenceTotals[SequenceTotalsLen][4] = price;
        SequenceTotals[SequenceTotalsLen][5] = buyerNum;
        SequenceTotalsLen++;
    }

    if (SequenceTotalsLen >= NUM_POSSIBLE_SEQUENCES)
    {
        printf("bad\n");
    }

    return retVal;
}

/*
 * Given a buyer (index into the SecretSeeds array), determine how much the
 * sequence starting at seqStart (in the Differences array) will yield
 */
int getProfit(int buyerIdx, int seqStart)
{
    int curSeq[SEQUENCE_LEN];
    int curSeqStart = 0;
    long long secretNum = SecretSeeds[buyerIdx];
    int prevPrice = secretNum % 10;
    int first = Differences[seqStart];
    int second = Differences[seqStart + 1];
    int third = Differences[seqStart + 2];
    int fourth = Differences[seqStart + 3];
    int numIterations = 0;
    for (int i = 0; i < 4; i++)
    {
        secretNum = evolveNum(secretNum);
        curSeq[i] = (secretNum % 10) - prevPrice;
        prevPrice = secretNum % 10;
        numIterations++;
    }
    while (numIterations < 2000 &&
           (first != curSeq[curSeqStart] ||
            second != curSeq[(curSeqStart + 1) % 4] ||
            third != curSeq[(curSeqStart + 2) % 4] ||
            fourth != curSeq[(curSeqStart + 3) % 4]))
    {
        secretNum = evolveNum(secretNum);
        curSeq[curSeqStart] = (secretNum % 10) - prevPrice;
        prevPrice = secretNum % 10;
        curSeqStart = (curSeqStart + 1) % 4;
        numIterations++;
    }

    if (first == curSeq[curSeqStart] &&
        second == curSeq[(curSeqStart + 1) % 4] &&
        third == curSeq[(curSeqStart + 2) % 4] &&
        fourth == curSeq[(curSeqStart + 3) % 4])
    {
        return prevPrice;
    }
    else
    {
        return 0;
    }
}

/*
 * Given an index in the Differences array, determine if we've already checked
 * the sequence of 4 differences, starting at that index.
 */
bool seenSequenceBefore(int idx)
{
    for (int i = 0; i < NumSequencesChecked; i++)
    {
        if (Differences[0] == SequencesChecked[i][0] &&
            Differences[1] == SequencesChecked[i][1] &&
            Differences[2] == SequencesChecked[i][2] &&
            Differences[3] == SequencesChecked[i][3])
        {
            return true;
        }
    }
    return false;
}

long long evolveNum(long long secretNum)
{
    long long temp = secretNum << 6;
    secretNum = temp ^ secretNum;
    secretNum &= 0xFFFFFF;

    temp = secretNum >> 5;
    secretNum = temp ^ secretNum;
    secretNum &= 0xFFFFFF;

    temp = secretNum << 11;
    secretNum = temp ^ secretNum;
    secretNum &= 0xFFFFFF;

    return secretNum;
}
