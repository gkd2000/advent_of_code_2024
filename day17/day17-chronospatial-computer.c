#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

/* My input for this problem is:

Register A: 28066687
Register B: 0
Register C: 0

Program: 2,4,1,1,7,5,4,6,0,3,1,4,5,5,3,0

*/

long long AReg;
long long BReg;
long long CReg;

/* Given an operand, this function returns the combo operand */
int getCombo(int operand)
{
    switch (operand)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        return operand;
        break;
    case 4:
        return AReg;
        break;
    case 5:
        return BReg;
        break;
    case 6:
        return CReg;
        break;
    default:
        break;
    }
    return -1;
}

/*
 * Numerator is value in A register. Denominator is 2^c, where c is the combo
 * operand of the instruction. Store numerator / denominator, truncated to an
 * integer, in the A register. This function modifies the contents of the
 * A register.
 */
void adv (int operand)
{
    // printf("AReg is %d, combo is %d\n", AReg, getCombo(operand));
    AReg = (long long) AReg / powl ((long double) 2, (long double) getCombo(operand));
}

/*
 * Calculate bitwise XOR of register B and the operand, then store the result
 * in register B.
 */
void bxl (int operand)
{
    BReg = BReg ^ operand;
}

/*
 * Calculate the value of the combo operand mod 8, then store this value to
 * register B.
 */
void bst (int operand)
{
    BReg = getCombo(operand) % 8;
}

/*
 * If register A is not 0, then return the literal operand, which is the
 * position to which to set the instruction pointer. Otherwise return -1.
 */
int jnz (int operand)
{
    if (AReg != 0)
    {
        return operand;
    }
    else
    {
        return -1;
    }
}

/*
 * Calculate the bitwise XOR of registers B and C, then store the result in
 * register C.
 */
void bxc ()
{
    BReg = BReg ^ CReg;
}

/* Calculate the value of the combo operand mod 8, then print it. */
void out (int operand, char *output)
{
    printf("%d,", getCombo(operand) % 8);
    // output[0] = (getCombo(operand) % 8) + '0';
    // output[1] = ',';
}

/*
 * Numerator is value in A register. Denominator is 2^c, where c is the combo
 * operand of the instruction. Store numerator / denominator, truncated to an
 * integer, in the B register. This function modifies the contents of the
 * B register.
 */
void bdv (int operand)
{
    BReg = (long long) AReg / powl ((long double) 2, (long double) getCombo(operand));
}

/*
 * Numerator is value in A register. Denominator is 2^c, where c is the combo
 * operand of the instruction. Store numerator / denominator, truncated to an
 * integer, in the C register. This function modifies the contents of the
 * C register.
 */
void cdv (int operand)
{
    CReg = (long long) AReg / powl ((long double) 2, (long double) getCombo(operand));
}

/* Run the program specific to my puzzle input and print the result */
void runProgram()
{
    AReg = 28066687;
    BReg = 0;
    CReg = 0;
    char *program = "2,4,1,1,7,5,4,6,0,3,1,4,5,5,3,0";
    char *instructionPtr = program;
    int pgmLen = strlen(program);
    int operand;
    int jumpToPos;
    char output[64] = {};
    output[0] = '\0';
    int outputLen = 0;

    while (instructionPtr - program < pgmLen)
    {
        jumpToPos = -1;
        operand = *(instructionPtr + 2) - '0';

        switch(*instructionPtr - '0')
        {
            case 0:
                adv(operand);
                break;
            case 1:
                bxl(operand);
                break;
            case 2:
                bst(operand);
                break;
            case 3:
                jumpToPos = jnz(operand);
                break;
            case 4:
                bxc();
                break;
            case 5:
                out(operand, &output[outputLen]);
                outputLen += 2;
                break;
            case 6:
                bdv(operand);
                break;
            case 7:
                cdv(operand);
                break;
        }
        /* Move instruction pointer appropriately */
        if (jumpToPos == -1)
        {
            instructionPtr += 4;
        }
        else
        {
            instructionPtr = program + (2 * jumpToPos);
        }
    }

    output[outputLen] = '\0';
    printf("%s\n", output);
}

int main (int argc, char **argv)
{
    /* Part 1 */
    printf("Part 1:\n  ");
    runProgram();

    /* Note that my program does the following:
     *  - Put AReg mod 8 value in BReg
     *  - Replace BReg value with BReg XOR 1
     *  - Put AReg / (2^BReg) in CReg
     *  - Calculate bitwise XOR of B and C, store in B
     *    - At this point, B holds bitwise XOR of ((A % 8) ^ 1)
     *      and A/(2^((A % 8) ^ 1))
     *  - Calculate AReg / 2^3 and store in AReg
     *  - Replace BReg value with BReg XOR 4
     *  - Output BReg mod 8
     *  - Jump to 0 if AReg nonzero, otherwise halt
     */
    /*
     * Part 2: This part relies heavily on my specific program, shown below
     * in goal with the commas removed. At each iteration, the program divides
     * the value in the A Register by 8, truncating the result to an integer,
     * which effectively bitshifts this value to the right by 3. Furthermore,
     * other instructions which affect the value printed take the A Register
     * value mod 8. Thus, we can iteratively construct the desired number 3 bits
     * (or one octal digit) at a time. We construct from the most significant
     * bit to the least, since more significant bits can impact the values
     * printed for less significant bits; thus, going most to least significant
     * avoids backtracking as much as possible. We maintain a list of candidate
     * prefixes. At each iteration, we add another octal digit (trying every
     * possibility from 0 to 7). If the octal digit produces the desired output,
     * then we add this new prefix to our list of candidates.
     */
    printf("Part 2:\n");
    char goal[] = "2411754603145530";
    long long bTemp;

    long long aRegSoFar = 0;
    long long candidateAReg = 0;
    int target, goalB;
    long long candidateArr[100];
    candidateArr[0] = 0;
    int candidateArrLen = 1;
    long long tempCandArr[100];
    int tempCandArrLen = 0;
    for (int i = strlen(goal) - 1; i >= 0; i--)
    {
        target = goal[i] - '0';

        /* Need rightmost three bits of b to XOR with 4 to be equal to target */
        goalB = (target ^ 4) % 8;

        tempCandArrLen = 0;
        /*
         * Iterate over the candidate array, adding new candidates to the temp
         * array as they are discovered
         */
        for (int candArrIdx = 0; candArrIdx < candidateArrLen; candArrIdx++)
        {
            aRegSoFar = candidateArr[candArrIdx];
            for (int j = 0; j < 8; j++)
            {
                candidateAReg = (aRegSoFar * 8) + j;

                /*
                 * Calculate the value that would be printed if this digit were
                 * appended. If it matches the goal, then add to the candidate
                 * array.
                 */
                bTemp = (candidateAReg ^ 1) % 8;
                if (goalB == (bTemp ^ (candidateAReg >> bTemp)) % 8)
                {
                    tempCandArr[tempCandArrLen] = candidateAReg;
                    tempCandArrLen++;
                }
            }
        }

        /*
         * Replace the candidate array by the temporary candidate array (move
         * on to the next digit in the sequence)
         */
        memcpy(candidateArr, tempCandArr, tempCandArrLen * sizeof(long long));
        candidateArrLen = tempCandArrLen;

        if (candidateArrLen == 0)
        {
            printf("no candidates found\n");
        }
    }

    /* Print results */
    for (int i = 0; i < candidateArrLen; i++)
    {
        printf("  %lld (%llo in octal)\n", candidateArr[i], candidateArr[i]);
    }
}