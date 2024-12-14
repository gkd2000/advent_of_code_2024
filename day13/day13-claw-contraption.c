#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

void solveEquation(long long ax, long long bx, long long cx, long long ay, long long by, long long cy, long long *aScalar,
                   long long *bScalar);
bool isInteger(double x);

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

    char *line = NULL;
    size_t len = 0;
    int currentLine = 0;
    int lineLen = 0;
    long long ax, ay, bx, by, cx, cy, aScalar, bScalar;
    char *xStr;
    char *yStr;
    long long total = 0;
    while ((lineLen = getline(&line, &len, filePointer)) != -1)
    {
        if (lineLen > 1)
        {
            xStr = strchr(line, 'X') + 2;
            yStr = strchr(xStr, 'Y') + 2;
        }

        switch (currentLine % 4)
        {
            case 0:
                ax = atoll(xStr);
                ay = atoll(yStr);
                break;
            case 1:
                bx = atoll(xStr);
                by = atoll(yStr);
                break;
            case 2:
                /*
                 * This does part 2. To make it do part 1 instead, remove the
                 * "+ 10000000000000"
                 */
                cx = atoll(xStr) + 10000000000000;
                cy = atoll(yStr) + 10000000000000;
                solveEquation(ax, bx, cx, ay, by, cy, &aScalar, &bScalar);
                total += 3*aScalar + bScalar;
                break;
            default:
                break;
        }
        currentLine++;
    }

    printf("total is %lld\n", total);
}

/*
 * Solve the system of equations
 *   (ax)(aScalar) + (bx)(bScalar) = cx
 *   (ay)(aScalar) + (by)(bScalar) = cy,
 * where ax, bx, cx, ay, by, cy are given and we are solving for aScalar
 * and bScalar.
 */
void solveEquation(long long ax, long long bx, long long cx, long long ay,
                   long long by, long long cy, long long *aScalar,
                   long long *bScalar)
{
    long long det = ax * by - bx * ay;
    double aScalarDouble = 0;
    double bScalarDouble = 0; 

    if (det != 0)
    {
        /*
         * A unique solution exists. Use the inverse matrix to get it.
         * Inverse matrix is 1/determinant times
         *  by  -bx
         *  -ay  ax
         *
         * So aScalar is 1/det * ((cx)(by) - (cy)(bx))
         *    bScalar is 1/det * ((cx)(-ay) + (cy)(ax))
         */
        aScalarDouble = (double) (cx * by - cy * bx) / (double) det;
        bScalarDouble = (double) (cy * ax - cx * ay) / (double) det;
        printf("aScalarDouble: %f, bScalarDouble: %f\n", aScalarDouble, bScalarDouble);

        if (isInteger(aScalarDouble) && isInteger(bScalarDouble))
        {
            *aScalar = (long long) aScalarDouble;
            *bScalar = (long long) bScalarDouble;
        }
        else
        {
            /* Unique solution is not integers */
            *aScalar = 0;
            *bScalar = 0;
        }
    }
    /*
     * Technically this isn't right. We choose B first, without comparing total
     * costs. Hoping that none of mine are linearly dependent anyway
     */
    else if (isInteger((double) cx / (double) bx) &&
             isInteger((double) cy / (double) by) &&
             cx / bx == cy / by)
    {
        *aScalar = 0;
        *bScalar = cx / bx;
    }
    else if (isInteger((double) cx / (double) ax) &&
             isInteger((double) cy / (double) ay) &&
             cx / ax == cy / ay)
    {
        *aScalar = cx / ax;
        *bScalar = 0;
    }
    else
    {
        *aScalar = 0;
        *bScalar = 0;
    }
}

bool isInteger(double x)
{
    return (x == (long long) x);
}