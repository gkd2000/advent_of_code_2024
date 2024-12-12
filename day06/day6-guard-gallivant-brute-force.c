#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define ROOM_DIM 130
#define NORTH 0x01
#define SOUTH 0x02
#define EAST  0x04
#define WEST  0x08
char RoomGrid[ROOM_DIM][ROOM_DIM];
int ObstaclesEncountered[ROOM_DIM][ROOM_DIM];


bool lookRightForObstacle(int row, int col, int rowDir, int colDir);
bool isInBounds(int row, int col);
char dirsToInt(int rowDir, int colDir);
bool runSimulation(int startRow, int startCol, int addedObsRow, int addedObsCol);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    /* Set the RoomGrid to 0, just in case */
    memset (RoomGrid, 0, sizeof(RoomGrid));
    memset (ObstaclesEncountered, 0, sizeof(ObstaclesEncountered));

    /* Attempt to open the file */
    char *fileName = argv[1];
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    /* Read the input file into the RoomGrid matrix */
    char *line = NULL;
    size_t len = 0;
    int currentRow = 0;
    char *guardPtr = NULL;
    int guardStartRow = -1;
    int guardStartCol = -1;
    while (getline(&line, &len, filePointer) != -1)
    {
        memcpy (&RoomGrid[currentRow][0], line, ROOM_DIM);

        /* See if the guard '^' is in this row, if we haven't already found her */
        if ( (guardStartRow == -1) && ((guardPtr = strchr (line, '^')) != NULL) )
        {
            guardStartRow = currentRow;
            guardStartCol = guardPtr - line;
        }
        currentRow++;
    }

    int totalPositions = 0;
    // Start with guard facing north because that's what my puzzle input is
    int rowDir = -1;
    int colDir = 0;
    int candidateObstacles[ROOM_DIM * ROOM_DIM];
    int obstaclePossibilities = 0;
    int guardRow = guardStartRow;
    int guardCol = guardStartCol;
    bool hasTurned = false;
    int total = 0;
    for (int i = 0; i < ROOM_DIM; i++)
    {
        for (int j = 0; j < ROOM_DIM; j++)
        {
            if (runSimulation(guardStartRow, guardStartCol, i, j))
            {
                // printf("%d %d\n", i, j);
                // printf("%c", RoomGrid[i][j]);
                total++;
                if (j == 50){
                    printf("%d %d\n", i, j);
                }
            }
        }
    }
    printf("total is %d\n", total);

    // while (isInBounds(guardRow, guardCol))
    // {
    //     if (RoomGrid[guardRow][guardCol] != 'X')
    //     {
    //         RoomGrid[guardRow][guardCol] = 'X';
    //         totalPositions++;
    //         // printf("%d %d\n", guardRow, guardCol);
    //     }

    //     if (!hasTurned && runSimulation(guardRow, guardCol))
    //     {
    //         candidateObstacles[(guardRow+rowDir)*ROOM_DIM + (guardCol+colDir)] = 1;
    //         obstaclePossibilities++;
    //     }

    //     /* Look right for any obstacles we've already encountered */
    //     if ( (lookRightForObstacle(guardRow, guardCol, rowDir, colDir)) &&
    //          (isInBounds(guardRow + rowDir, guardCol + colDir)) &&
    //          ((guardRow + rowDir != guardStartRow) || (guardCol + colDir != guardStartCol)))
    //     {
    //         if (candidateObstacles[(guardRow+rowDir)*ROOM_DIM + (guardCol+colDir)] != 1)
    //         {
    //             // printf("%d %d\n", guardRow+rowDir, guardCol+colDir);
    //             candidateObstacles[(guardRow+rowDir)*ROOM_DIM + (guardCol+colDir)] = 1;
    //             obstaclePossibilities++;
    //         }
    //     }

    //     if (RoomGrid[guardRow + rowDir][guardCol + colDir] == '#')
    //     {
    //         // printf("row %d dir %d, col %d dir %d\n", guardRow + rowDir, rowDir,
    //         //        guardCol + colDir, colDir);
    //         hasTurned = true;
    //         // Record this as an obstacle encountered and record the direction we were going when we hit it
    //         ObstaclesEncountered[guardRow + rowDir][guardCol + colDir] |= dirsToInt(rowDir, colDir);
    //         // Obstacle! Rotate the guard 90 degrees to the right
    //         if (rowDir != 0)
    //         {
    //             colDir = -rowDir;
    //             rowDir = 0;
    //         }
    //         else
    //         {
    //             rowDir = colDir;
    //             colDir = 0;
    //         }
    //     }
    //     else
    //     {
    //         // No obstacle. Update guard position
    //         guardRow += rowDir;
    //         guardCol += colDir;
    //     }
    // }

    // printf("total positions occupied is %d\n", totalPositions);
    // printf("total obstacle possibilities is %d\n", obstaclePossibilities);

    // for (int i = 0; i < ROOM_DIM; i++)
    // {
    //     for (int j = 0; j < ROOM_DIM; j++)
    //     {
    //         if (ObstaclesEncountered[i][j] == 0)
    //         {
    //             printf(".");
    //         }
    //         else
    //         {
    //             printf("#");
    //         }
    //         // printf("%d", ObstaclesEncountered[i][j]);
    //     }
    //     printf("\n");
    // }
    // printf("guard is at row %d, col %d\n", guardRow, guardCol);

}

/*
 * I think there's a special condition before the guard takes any turns where
 * you might be able to get a cycle by placing obstacles directly in her path.
 * Let's assume first that these cycles are just rectangles (though this might
 * not be true). If so, then
 */
bool runSimulation(int startRow, int startCol, int addedObsRow, int addedObsCol)
{
    if ((startRow == addedObsRow && startCol == addedObsCol) || (RoomGrid[addedObsRow][addedObsCol] == '#'))
    {
        return false;
    }
    int guardRow = startRow;
    int guardCol = startCol;
    // Start with guard facing north because that's what my puzzle input is
    int rowDir = -1;
    int colDir = 0;
    char simulationRoomGrid[ROOM_DIM][ROOM_DIM];
    memcpy(simulationRoomGrid, RoomGrid, sizeof(simulationRoomGrid));
    simulationRoomGrid[addedObsRow][addedObsCol] = '#';
    int currentDir;
    int foundCycle = false;

    while (isInBounds(guardRow, guardCol) && !foundCycle)
    {
        currentDir = dirsToInt(rowDir, colDir);
        if (simulationRoomGrid[guardRow][guardCol] == '.' || simulationRoomGrid[guardRow][guardCol] == '^')
        {
            simulationRoomGrid[guardRow][guardCol] = currentDir;
        }
        else if ((simulationRoomGrid[guardRow][guardCol] & currentDir) != 0)
        {
            // printf("%d ", simulationRoomGrid[guardRow][guardCol]);
            // We've visited this position going this direction. We're in a loop.
            foundCycle = true;
        }
        else
        {
            // We've visited this position going a different direction
            simulationRoomGrid[guardRow][guardCol] |= currentDir;
        }

        if (isInBounds(guardRow + rowDir, guardCol + colDir) && simulationRoomGrid[guardRow + rowDir][guardCol + colDir] == '#')
        {
            // Obstacle! Rotate the guard 90 degrees to the right
            if (rowDir != 0)
            {
                colDir = -rowDir;
                rowDir = 0;
            }
            else
            {
                rowDir = colDir;
                colDir = 0;
            }
        }
        else
        {
            // No obstacle. Update guard position
            guardRow += rowDir;
            guardCol += colDir;
        }
    }
    return foundCycle;

}

/* Given a direction -1, 0, 1 for the row and column, translate this to a cardinal direction */
char dirsToInt(int rowDir, int colDir)
{
    if (rowDir == -1)
    {
        return NORTH;
    }
    else if (rowDir == 1)
    {
        return SOUTH;
    }
    else if (colDir == -1)
    {
        return WEST;
    }
    else if (colDir == 1)
    {
        return EAST;
    }
    else
    {
        printf("error");
    }
    return 0;
}

bool isInBounds(int row, int col)
{
    return (row >= 0) && (row < ROOM_DIM) && (col >= 0) && (col < ROOM_DIM);
}

bool lookRightForObstacle(int row, int col, int rowDir, int colDir)
{
    bool retVal = false;
    if (rowDir != 0)
    {
        colDir = -rowDir;
        rowDir = 0;
    }
    else
    {
        rowDir = colDir;
        colDir = 0;
    }

    int dirChar = dirsToInt(rowDir, colDir);
    while ( (!retVal) && (isInBounds(row, col)) )
    {
        if ((ObstaclesEncountered[row][col] & dirChar) != 0)
        {
            retVal = true;
        }
        else
        {
            row += rowDir;
            col += colDir;
        }
    }

    return retVal;
}
