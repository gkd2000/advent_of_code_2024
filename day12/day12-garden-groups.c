#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define PLOT_DIM 5
#define UPPER_TO_LOWER_SHIFT 32
char PlotGrid[PLOT_DIM][PLOT_DIM];
// int Areas[MAX_PLOT_TYPES];
// int Perimeters[MAX_PLOT_TYPES];

typedef struct plotData
{
    long area;
    long perimeter;
    long sides;
} plotData_t;

plotData_t *PlotData[PLOT_DIM][PLOT_DIM];

/*
 * To do: need to handle interior regions. I want to use the PlotData matrix
 * and at every grid point, put a pointer to its plot data. Also add a check
 * to see if when I calculated the sides, I did it for an island. If I did, then
 * need to add the sides calculated to whatever the exterior region is.
 */

plotData_t processRegion(int row, int col);
void processRegionKernel (int row, int col, char plotType, plotData_t *thisPlot);
void processRegionSidesKernel(int row, int col, int prevRow, int prevCol,
                              char plotType, plotData_t *thisPlot);
void printPlotGrid();
long getNumSides (int row, int col);
bool isInBounds (int row, int col);
void turnRight (int *rowDir, int *colDir);
void turnLeft (int *rowDir, int *colDir);

int main (int argc, char **argv)
{
    /* Validate argument */
    if (argc < 2)
    {
        printf("Please provide an input file\n");
        return 0;
    }

    /* Set the arrays to 0, just in case */
    memset (PlotGrid, 0, sizeof(PlotGrid));
    // memset (Areas, 0, sizeof(Areas));
    // memset (Perimeters, 0, sizeof(Perimeters));

    /* Attempt to open the file */
    char *fileName = argv[1];
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    /* Read the input file into the PlotGrid matrix */
    char *line = NULL;
    size_t len = 0;
    int currentRow = 0;
    while (getline(&line, &len, filePointer) != -1)
    {
        memcpy (&PlotGrid[currentRow][0], line, PLOT_DIM);
        currentRow++;
    }

    long totalWithPerimeter = 0;
    long totalWithSides = 0;
    long thisTotal;
    plotData_t thisData = {0, 0, 0};
    for (int i = 0; i < PLOT_DIM; i++)
    {
        for (int j = 0; j < PLOT_DIM; j++)
        {
            if (PlotGrid[i][j] >= 'A' && PlotGrid[i][j] <= 'Z')
            {
                char saved = PlotGrid[i][j];
                thisData = processRegion(i, j);
                totalWithPerimeter += thisData.area * thisData.perimeter;
                totalWithSides += thisData.area * thisData.sides;
                printf("Region %c with area %ld, perimeter %ld, sides %ld\n", saved, thisData.area, thisData.perimeter, thisData.sides);
            }
        }
    }

    printf("total with perimeter is %ld\ntotal with sides is %ld\n", totalWithPerimeter, totalWithSides);

}

plotData_t processRegion(int row, int col)
{
    plotData_t perimeterCalcs = {0, 0, 0};
    // char savedPlotGrid[PLOT_DIM][PLOT_DIM];
    // memcpy(savedPlotGrid, PlotGrid, sizeof(PlotGrid));
    perimeterCalcs.sides = getNumSides(row, col);
    processRegionKernel(row, col, PlotGrid[row][col], &perimeterCalcs);

    // plotData_t sideCalcs = {0, 0, 0};
    // memcpy(PlotGrid, savedPlotGrid, sizeof(PlotGrid));
    // processRegionSidesKernel(row, col, row, col, PlotGrid[row][col], &sideCalcs);

    // if (sideCalcs.area != perimeterCalcs.area)
    // {
    //     printf("warning, area calculation doesn't match. Perimeter gave %ld and sides gave %ld\n",
    //             perimeterCalcs.area, sideCalcs.area);
    // }
    // printf("sides: %ld\n", sideCalcs.sides);
    // perimeterCalcs.sides = sideCalcs.sides;
    return perimeterCalcs;
}

/* Get the number of sides of the plot containing index row, col */
long getNumSides (int row, int col)
{
    char plotType = PlotGrid[row][col];
    /* Go to a bottom boundary */
    while (isInBounds(row, col) && PlotGrid[row][col] == plotType)
    {
        row++;
    }
    printf("After iterating down: at row %d, col %d\n", row, col);
    int startRow = row;
    int startCol = col;
    int movingRowDir = 0;
    int movingColDir = 1;
    int leftHandRowDir = -1;
    int leftHandColDir = 0;
    long numSides = 0;
    // row += movingRowDir;
    // col += movingColDir;

    // while(row != startRow || col != startCol)
    bool haveMoved = false;
    // for (int i = 0; i < 10; i++)
    do
    {
        printf("    row %d, col %d\n", row, col);
        if ( ((!isInBounds(row + movingRowDir, col + movingColDir)) ||
              (PlotGrid[row + movingRowDir][col + movingColDir] != plotType)) &&
            ((isInBounds(row + movingRowDir + leftHandRowDir, col + movingColDir + leftHandColDir) &&
            PlotGrid[row + movingRowDir + leftHandRowDir][col + movingColDir + leftHandColDir] == plotType)) )
        {
            /* Still in the perimeter. Advance by one step */
            row += movingRowDir;
            col += movingColDir;
            haveMoved = true;
        }
        else if ((isInBounds(row + movingRowDir, col + movingColDir)) &&
                 (PlotGrid[row + movingRowDir][col + movingColDir] == plotType))
        {
            /*
             * Advancing one would run us into a region with the same plot type.
             * See if it's actually the same region
             */
            if (PlotGrid[row + movingRowDir + leftHandRowDir][col + movingColDir + leftHandColDir] == plotType)
            {
                /* Yep, same region. Turn right to continue */
                turnRight(&movingRowDir, &movingColDir);
                turnRight(&leftHandRowDir, &leftHandColDir);
                // row += movingRowDir;
                // col += movingColDir;
                numSides++;
            }
            else
            {
                /*
                 * No, we've run into a separate plot kitty corner to ours.
                 * Advance past it and then turn left to continue
                 */
                // row += movingRowDir + leftHandRowDir;
                // col += movingColDir + leftHandColDir;
                row += movingRowDir;
                col += movingColDir;
                turnLeft(&movingRowDir, &movingColDir);
                turnLeft(&leftHandRowDir, &leftHandColDir);
                numSides++;
                haveMoved = true;
            }
        }
        else
        {
            /*
             * Advancing would put us in a position where the region is no 
             * longer on our left. Turn left to continue.
             */
            row += movingRowDir;
            col += movingColDir;
            turnLeft(&movingRowDir, &movingColDir);
            turnLeft(&leftHandRowDir, &leftHandColDir);
            numSides++;
            haveMoved = true;
        }
    } while (!haveMoved || (row != startRow || col != startCol));

    return numSides;
}

void turnRight (int *rowDir, int *colDir)
{
    if (*rowDir == 0)
    {
        *rowDir = *colDir;
        *colDir = 0;
    }
    else
    {
        *colDir = -(*rowDir);
        *rowDir = 0;
    }
}

void turnLeft (int *rowDir, int *colDir)
{
    if (*rowDir == 0)
    {
        *rowDir = -(*colDir);
        *colDir = 0;
    }
    else
    {
        *colDir = *rowDir;
        *rowDir = 0;
    }
}

/*
 * Idea: get rid of the second recursive method. Instead, whenever we process a
 * new region, first walk around that region counting sides. Do this by having
 * an ordered pair that we keep iterating: if we're moving left around the boundary,
 * then whenever we get to a square that no longer borders the region in question,
 * the boundary either turns up or down, but not both. At each turn, increment the
 * counter for number of sides.
 */

void processRegionKernel (int row, int col, char plotType, plotData_t *thisPlot)
{
    if (!isInBounds(row, col) ||
        (PlotGrid[row][col] != plotType &&
         PlotGrid[row][col] != plotType + UPPER_TO_LOWER_SHIFT))
    {
        /*
         * We are out of the current region (either we left the grid or we are
         * on a plot of a different type)
         */
        thisPlot->perimeter += 1;
    }
    else if (PlotGrid[row][col] == plotType)
    {
        thisPlot->area += 1;

        /* Mark this square as visited */
        PlotGrid[row][col] = plotType + UPPER_TO_LOWER_SHIFT;
        PlotData[row][col] = thisPlot;

        /* Recursive calls */
        processRegionKernel(row + 1, col, plotType, thisPlot);
        processRegionKernel(row - 1, col, plotType, thisPlot);
        processRegionKernel(row, col + 1, plotType, thisPlot);
        processRegionKernel(row, col - 1, plotType, thisPlot);
    }
}

void processRegionSidesKernel(int row, int col, int prevRow, int prevCol,
                              char plotType, plotData_t *thisPlot)
{
    if (!isInBounds(row, col) ||
        (PlotGrid[row][col] != plotType &&
         PlotGrid[row][col] != plotType + UPPER_TO_LOWER_SHIFT))
    {
        thisPlot->sides += 1;
        printf("found a side. row: %d, col: %d, prevRow: %d, prevCol: %d\n",
                row, col, prevRow, prevCol);
        /*
         * We are out of the current region (either we left the grid or we are
         * on a plot of a different type)
         */
        if (row - prevRow == 1 || row - prevRow == -1)
        {
            for (int searchCol = col + 1; searchCol >= 0 && searchCol < PLOT_DIM &&
                                          PlotGrid[prevRow][searchCol] == plotType &&
                                          (!isInBounds(row, searchCol) || 
                                           (PlotGrid[row][searchCol] != plotType &&
                                            PlotGrid[row][searchCol] != plotType + UPPER_TO_LOWER_SHIFT));
                 searchCol++)
            {
                thisPlot->area += 1;
                PlotGrid[prevRow][searchCol] = plotType + UPPER_TO_LOWER_SHIFT;
                printPlotGrid();
            }

            for (int searchCol = col - 1; searchCol >= 0 && searchCol < PLOT_DIM &&
                                          PlotGrid[prevRow][searchCol] == plotType&&
                                          (!isInBounds(row, searchCol) || 
                                           (PlotGrid[row][searchCol] != plotType &&
                                            PlotGrid[row][searchCol] != plotType + UPPER_TO_LOWER_SHIFT));
                 searchCol--)
            {
                thisPlot->area += 1;
                PlotGrid[prevRow][searchCol] = plotType + UPPER_TO_LOWER_SHIFT;
                printPlotGrid();
            }
        }
        else
        {
            for (int searchRow = row + 1; searchRow >= 0 && searchRow < PLOT_DIM &&
                                          PlotGrid[searchRow][prevCol] == plotType &&
                                          (!isInBounds(searchRow, col) ||
                                           (PlotGrid[searchRow][col] != plotType &&
                                            PlotGrid[searchRow][col] != plotType + UPPER_TO_LOWER_SHIFT));
                 searchRow++)
            {
                thisPlot->area += 1;
                PlotGrid[searchRow][prevCol] = plotType + UPPER_TO_LOWER_SHIFT;
                printPlotGrid();
            }

            for (int searchRow = row - 1; searchRow >= 0 && searchRow < PLOT_DIM &&
                                          PlotGrid[searchRow][prevCol] == plotType &&
                                          (!isInBounds(searchRow, col) ||
                                           (PlotGrid[searchRow][col] != plotType &&
                                            PlotGrid[searchRow][col] != plotType + UPPER_TO_LOWER_SHIFT));
                 searchRow--)
            {
                thisPlot->area += 1;
                PlotGrid[searchRow][prevCol] = plotType + UPPER_TO_LOWER_SHIFT;
                printPlotGrid();
            }
        }
    }
    else if (PlotGrid[row][col] == plotType)
    {
        thisPlot->area += 1;

        /* Mark this square as visited */
        PlotGrid[row][col] = plotType + UPPER_TO_LOWER_SHIFT;
        printPlotGrid();

        /* Recursive calls */
        processRegionSidesKernel(row + 1, col, row, col, plotType, thisPlot);
        processRegionSidesKernel(row - 1, col, row, col, plotType, thisPlot);
        processRegionSidesKernel(row, col + 1, row, col, plotType, thisPlot);
        processRegionSidesKernel(row, col - 1, row, col, plotType, thisPlot);
    }
}

void printPlotGrid()
{
    for (int i = 0; i < PLOT_DIM; i++)
    {
        for (int j = 0; j < PLOT_DIM; j++)
        {
            printf("%c ", PlotGrid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

bool isInBounds (int row, int col)
{
    return ( (row >= 0) && (row < PLOT_DIM) && (col >= 0) && (col < PLOT_DIM) );
}