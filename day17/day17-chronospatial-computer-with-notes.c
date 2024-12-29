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

bool runSimulation(long long candidate);

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

void printState(char *instructionPtr, int operand)
{
    printf("A Register: %lld\nB Register: %lld\nC Register: %lld\nInstruction Pointer: %c\nOperand: %d\n\n",
            AReg, BReg, CReg, *instructionPtr, operand);
}

void runProgram()
{
    char *program = "2,4,1,1,7,5,4,6,0,3,1,4,5,5,3,0";
    char *instructionPtr = program;
    int pgmLen = strlen(program);
    int operand;
    int jumpToPos;
    char output[64] = {};
    output[0] = '\0';
    int outputLen = 0;

    // while ((instructionPtr - program < pgmLen) &&
    //         strncmp(program, output, outputLen) == 0)
    while (instructionPtr - program < pgmLen)
    {
        jumpToPos = -1;
        operand = *(instructionPtr + 2) - '0';
        // printf("Instruction pointer: %c, operand %d, AReg: %d, BReg: %d, CReg: %d\n",
        //         *instructionPtr, operand, AReg, BReg, CReg);
        // printState(instructionPtr, operand);
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
        // printf("output is %s\n", output);
    }

    output[outputLen] = '\0';
    printf("%s\n", output);
}

int main (int argc, char **argv)
{
    AReg = 28066687;
    BReg = 0;
    CReg = 0;
    runProgram();

    /*
    - Put AReg mod 8 value in BReg
    - Replace BReg value with BReg XOR 1
    - Put AReg / (2^BReg) in CReg
    - Calculate bitwise XOR of B and C, store in B
      - At this point, B holds bitwise XOR of ((A % 8) ^ 1)
        and A/(2^((A % 8) ^ 1))
    - Calculate AReg / 2^3 and store in AReg
    - Replace BReg value with BReg XOR 4
    - Output BReg mod 8
    - Jump to 0 if AReg nonzero, otherwise halt
    */

    /* (A % 8) means take the 3 least significant bits of A
     * A ^ 1 means if the least significant bit of A is 1, then turn it off. If
     * it's 0, then turn it on. So flip the least significant bit. So if A is
     * odd, then subtract 1. If A is even, then add 1.
     */

    /*
     * First digit we need is 2. So BReg must end in 110 before we XOR with 4
     */

    /* Number of digits printed by program increases by 1 for every power of 8.
     * So if AReg is between 8^1 and 8^2, we get 2 digits.
     * So we need AReg to be between 8^15 and 8^16
     */

    char goal[] = "2411754603145530";
    long long b;
    long long bTemp;
    int idx = 0;
    // 35184372088832 = 8^15 = 1000000000000000 in octal. we need our number to
    // begin with 5 in its octal representation, so look at numbers between
    // 5000000000000000 (in octal) and 6000000000000000 (in octal). this is
    // 5 * 8^15 and 6 * 8^15
    // long long start = 5 * 35184372088832;
    // long long end = 6 * 35184372088832;
    long long start = 202972175280682;
    long long end  = 202972175280683;
    bool foundAns = false;
    for (; start < end && !foundAns; start++)
    {
        // foundAns = true;
        AReg = start;
        idx = 0;
        printf("%llo: ", AReg);
        /*
         * Since dividing by 8 is bitshifting to the right, could we do this non-iteratively,
         * by looking at the bits in AReg initially and corresponding each chunk
         * of three bits to an outputted number?
         * Seems like if the most significant digit in octal is 5, then our final
         * digit printed will be 0.
         * We can predict what bTemp will be based on the octal digit (because
         * there is no XORing with bitshifted values of AReg yet).
         * To get 2 to be the first value printed,
         * (b ^ 4) % 8 must be 2, so if we take the rightmost three bits of b and
         * XOR them with binary 100, we want to get 010. So we need b to have
         * rightmost three bits 110. This means that bTemp ^ (AReg >> bTemp) has
         * rightmost three bits 110. So given the rightmost three bits of bTemp,
         * we can determine what the three bits of AReg which are bTemp from the
         * right must be. Maybe need to start from the last number outputted and
         * go from there?
         * 
         * If the most significant digit in octal is 5, then our final digit
         * printed will be 0.
         * Need the second to last digit to be 3, so need (b ^ 4) % 8 to be 3.
         * So b has rightmost three bits 000.
         */
        do
        {
            // printf("%d\n", idx);
            bTemp = ((AReg % 8) ^ 1);
            // printf("\n  bTemp: %lld, ", bTemp);
            b = bTemp ^ (AReg >> bTemp);
            // printf("b: %lld, ", b);
            // b = bTemp ^ (AReg / (1 << bTemp));
            // b = bTemp ^ (long long) (AReg / powl((long double) 2, (long double) bTemp));
            // printf("%lld,", (b ^ 4) % 8);
            if ((b ^ 4) % 8 != goal[idx] - '0'){
                AReg = 0;
                // foundAns = false;
            }
            else
            {
                printf("%lld, ", (b ^ 4) % 8);
            }
            AReg = AReg >> 3;
            idx++;
        } while (AReg != 0);
        printf("\n");
    }

    /*
     * Since dividing by 8 is bitshifting to the right, could we do this non-iteratively,
     * by looking at the bits in AReg initially and corresponding each chunk
     * of three bits to an outputted number?
     * Seems like if the most significant digit in octal is 5, then our final
     * digit printed will be 0.
     * We can predict what bTemp will be based on the octal digit (because
     * there is no XORing with bitshifted values of AReg yet).
     * To get 2 to be the first value printed,
     * (b ^ 4) % 8 must be 2, so if we take the rightmost three bits of b and
     * XOR them with binary 100, we want to get 010. So we need b to have
     * rightmost three bits 110. This means that bTemp ^ (AReg >> bTemp) has
     * rightmost three bits 110. So given the rightmost three bits of bTemp,
     * we can determine what the three bits of AReg which are bTemp from the
     * right must be. Maybe need to start from the last number outputted and
     * go from there?
     * 
     * If the most significant digit in octal is 5, then our final digit
     * printed will be 0.
     * Need the second to last digit to be 3, so need (b ^ 4) % 8 to be 3.
     * So b has rightmost three bits 111.
     */

    long long aRegSoFar = 0;
    long long candidateAReg = 0;
    int target, goalB;
    long long candidateArr[100];
    long long nextCandArr[100];
    candidateArr[0] = 0;
    int candidateArrLen = 1;
    int nextCandArrLen = 0;
    for (int i = strlen(goal) - 1; i >= 0; i--)
    {
        target = goal[i] - '0';

        // Need rightmost three bits of b to XOR with 4 to make target
        goalB = (target ^ 4) % 8;

        // bTemp = (AReg ^ 1) % 8
        // b = bTemp ^ (AReg >> bTemp)
        bool foundDigit = false;
        nextCandArrLen = 0;
        for (int candArrIdx = 0; candArrIdx < candidateArrLen; candArrIdx++)
        {
            aRegSoFar = candidateArr[candArrIdx];
            printf("aRegSoFar: %llo\n", aRegSoFar);
            for (int j = 0; j < 8; j++)
            {
                candidateAReg = (aRegSoFar * 8) + j;
                printf("  candidateAReg: %llo\n", candidateAReg);
                bTemp = (candidateAReg ^ 1) % 8;
                if (goalB == (bTemp ^ (candidateAReg >> bTemp)) % 8)
                {
                    nextCandArr[nextCandArrLen] = candidateAReg;
                    nextCandArrLen++;
                    printf("    adding candidate to array\n");
                }
            }
        }
        memcpy(candidateArr, nextCandArr, nextCandArrLen * sizeof(long long));
        candidateArrLen = nextCandArrLen;

        if (candidateArrLen == 0)
        {
            printf("no candidates found\n");
        }
        // if (foundDigit)
        // {
        //     aRegSoFar = candidateAReg;
        //     printf("%llo\n", aRegSoFar);
        // }
        // else
        // {
        //     printf("did not find a suitable digit\n");
        // }
    }

    for (int i = 0; i < candidateArrLen; i++)
    {
        printf("%lld (%llo in octal)\n", candidateArr[i], candidateArr[i]);
    }

    // 5600_42_74024_45
    long long baseVal = 05600042074024045;
                        //  5600042074024045
    long long pos1 = 0100; // first blank is in 8^2 position
    long long pos2 = 0100000000;
    long long pos3 = 0100000000000;
    long long pos4 = 010000000000000;
    long long pos5 = 01000000000000;
    long long pos6 = 0100000;
    long long candidate = 0;
    foundAns = false;
    // pos1 must be even in order to get a 2 in the first printing
    for (int i = 0; i < 8 && !foundAns; i+=2)
    {
        for (int j = 0; j < 8 && !foundAns; j++)
        {
            for (int k = 0; k < 8 && !foundAns; k++)
            {
                // for (int l = 0; l < 2 && !foundAns; l++)
                // {
                //     for (int m = 0; m < 2 && !foundAns; m++)
                //     {
                //         for (int n = 0; n < 2 && !foundAns; n++)
                //         {
                            // candidate = baseVal + (pos1 * i) + (pos2 * j) + (pos3 * k) + (pos4 * l) + (pos5 * m) + (pos6 * n);
                            candidate = baseVal + (pos1 * i) + (pos2 * j) + (pos3 * k);
                            // if (runSimulation(candidate))
                            // {
                            //     foundAns = true;
                            // }
                //         }
                //     }
                // }
            }
        }
    }

    if (foundAns)
    {
        printf("answer is %lld (in octal: %llo)\n", candidate, candidate);
    }
    else
    {
        printf("did not find an answer");
    }

    char *program = "2,4, 1,1, 7,5, 4,6, 0,3, 1,4, 5,5, 3,0";
    char *instructionPtr = program;
    int pgmLen = strlen(program);
    int operand;
    int jumpToPos;
    long long counter = 0;

    printf("%lld\n", AReg);

    printf("A Register is %lld\n", AReg);

    AReg = 28066687;
    BReg = 0;
    CReg = 0;
    while (instructionPtr - program < pgmLen)
    {
        jumpToPos = -1;
        operand = *(instructionPtr + 2) - '0';
        // printf("Instruction pointer: %c, operand %d, AReg: %d, BReg: %d, CReg: %d\n",
        //         *instructionPtr, operand, AReg, BReg, CReg);
        // printState(instructionPtr, operand);
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
                out(operand, NULL);
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
    printf("\n");
}

bool runSimulation(long long candidate)
{
    char goal[] = "2411754603145530";
    long long b;
    long long bTemp;
    int idx = 0;
    bool foundAns = true;
    printf("\ncandidate is %llo: ", candidate);

    do
    {
        // printf("%d\n", idx);
        bTemp = ((candidate % 8) ^ 1);
        printf("\n  bTemp: %lld, ", bTemp);
        b = bTemp ^ (candidate >> bTemp);
        printf("b: %lld, ", b);
        // b = bTemp ^ (candidate / (1 << bTemp));
        // b = bTemp ^ (long long) (candidate / powl((long double) 2, (long double) bTemp));
        // printf("%lld,", (b ^ 4) % 8);
        if ((b ^ 4) % 8 != goal[idx] - '0'){
            candidate = 0;
            foundAns = false;
        }
        printf("print: %lld, ", (b ^ 4) % 8);
        candidate = candidate >> 3;
        idx++;
    } while (candidate != 0);

    return foundAns;
}