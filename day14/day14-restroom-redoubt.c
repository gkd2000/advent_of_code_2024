#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// #define ROOM_WIDTH 11
// #define ROOM_HEIGHT 7
// #define NUM_ROBOTS 12
#define ROOM_WIDTH 101
#define ROOM_HEIGHT 103
#define NUM_ROBOTS 500
#define NUM_ITERATIONS 100

typedef struct
{
    int x;
    int y;
} point_t;

char PrintGrid[ROOM_HEIGHT][ROOM_WIDTH];
point_t CurrentState[NUM_ROBOTS];
point_t Velocities[NUM_ROBOTS];

enum Quadrants
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    NoQuadrant
};

int getQuadrant (int x, int y);
void iterateBoard();
void printBoard(int numIterations);

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
    int xStart, yStart, xVel, yVel, xEnd, yEnd;
    int total = 0;
    int quadrantTotals[4] = {0};
    while ((lineLen = getline(&line, &len, filePointer)) != -1)
    {

        /* Get the starting position and velocity */
        line = strchr(line, '=');
        xStart = atoi(line + 1);
        line = strchr(line, ',');
        yStart = atoi(line + 1);
        line = strchr(line, '=');
        xVel = atoi(line + 1);
        line = strchr(line, ',');
        yVel = atoi(line + 1);

        point_t thisPoint = {xStart, yStart};
        point_t thisVel = {xVel, yVel};
        CurrentState[currentLine] = thisPoint;
        Velocities[currentLine] = thisVel;

        xEnd = (xStart + (NUM_ITERATIONS * xVel)) % ROOM_WIDTH;
        if (xEnd < 0)
        {
            xEnd += ROOM_WIDTH;
        }
        yEnd = (yStart + (NUM_ITERATIONS * yVel)) % ROOM_HEIGHT;
        if (yEnd < 0)
        {
            yEnd += ROOM_HEIGHT;
        }

        // printf("%d %d, quadrant %d\n", xEnd, yEnd, getQuadrant(xEnd, yEnd));

        quadrantTotals[getQuadrant (xEnd, yEnd)] += 1;
        currentLine++;
    }

    total = quadrantTotals[0] * quadrantTotals[1] * quadrantTotals[2] * quadrantTotals[3];
    printf("total is %d\n", total);

    for (int i = 0; i < 10000; i++)
    {
        if (i % 101 == 14 || i % 103 == 64)
        {
            printBoard(i);
        }
        iterateBoard();
    }

    // char usrInput[5];
    // int boardNum = 100;
    // do
    // {
    //     printBoard(boardNum);
    //     iterateBoard();
    //     boardNum++;
    //     fgets(usrInput, 4, stdin);
    // } while(usrInput[0] != 'q');
}

void iterateBoard()
{
    int newX;
    int newY;
    for (int i = 0; i < NUM_ROBOTS; i++)
    {
        newX = (CurrentState[i].x + Velocities[i].x) % ROOM_WIDTH;
        if (newX < 0)
        {
            newX += ROOM_WIDTH;
        }
        newY = (CurrentState[i].y + Velocities[i].y) % ROOM_HEIGHT;
        if (newY < 0)
        {
            newY += ROOM_HEIGHT;
        }
        CurrentState[i].x = newX;
        CurrentState[i].y = newY;
    }
}

void printBoard(int numIterations)
{
    memset(PrintGrid, ' ', sizeof(PrintGrid));

    int curX;
    int curY;
    printf("Board %d\n\n", numIterations);
    for (int i = 0; i < NUM_ROBOTS; i++)
    {
        curX = CurrentState[i].x;
        curY = CurrentState[i].y;
        if (PrintGrid[curY][curX] == ' ')
        {
            PrintGrid[curY][curX] = '1';
        }
        else
        {
            PrintGrid[curY][curX] += 1;
        }
    }

    for (int row = 0; row < ROOM_HEIGHT; row++)
    {
        for (int col = 0; col < ROOM_WIDTH; col++)
        {
            printf("%c", PrintGrid[row][col]);
        }
        printf("\n");
    }
    printf("\n\n");
}

int getQuadrant (int x, int y)
{
    int xMidPt = (ROOM_WIDTH - 1) / 2;
    int yMidPt = (ROOM_HEIGHT - 1) / 2;

    if (x < xMidPt && y < yMidPt)
    {
        return TopLeft;
    }
    else if (x > xMidPt && y < yMidPt)
    {
        return TopRight;
    }
    else if (x < xMidPt && y > yMidPt)
    {
        return BottomLeft;
    }
    else if (x > xMidPt && y > yMidPt)
    {
        return BottomRight;
    }
    else
    {
        return NoQuadrant;
    }
}