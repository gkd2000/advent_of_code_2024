#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define PLOT_DIM 140
#define UPPER_TO_LOWER_SHIFT 32
char PlotGrid[PLOT_DIM][PLOT_DIM];

typedef struct plotData
{
    long area;
    long perimeter;
    int minRow;
    int maxRow;
    int minCol;
    int maxCol;
    int regionId;
} plotData_t;

plotData_t *PlotData[PLOT_DIM][PLOT_DIM];
int RegionIdCtr = 0;

plotData_t processRegion(int row, int col);
void processRegionKernel (int row, int col, char plotType, plotData_t *thisPlot);
void printPlotGrid();
bool isInBounds (int row, int col);
long getSides(int minRow, int maxRow, int minCol, int maxCol, int id);

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
    memset (PlotData, 0, sizeof(PlotData));

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
    // printPlotGrid();

    long perimeterTotal = 0;
    long sideTotal = 0;

    /*
     * Keep track of the regions we've seen so that we don't double count any
     * sides
     */
    bool *seenRegions = malloc(sizeof(bool) * RegionIdCtr);
    memset (seenRegions, 0, sizeof(sizeof(bool) * RegionIdCtr));
    plotData_t thisPlotData;
    long sides;
    for (int i = 0; i < PLOT_DIM; i++)
    {
        for (int j = 0; j < PLOT_DIM; j++)
        {
            if ( (PlotGrid[i][j] >= 'A' && PlotGrid[i][j] <= 'Z') ||
                 (!seenRegions[PlotData[i][j]->regionId]) )
            {
                thisPlotData = processRegion(i, j);
                sides = getSides(thisPlotData.minRow, thisPlotData.maxRow,
                                 thisPlotData.minCol, thisPlotData.maxCol,
                                 thisPlotData.regionId);
                perimeterTotal += thisPlotData.area * thisPlotData.perimeter;
                sideTotal += thisPlotData.area * sides;
                seenRegions[thisPlotData.regionId] = true;
            }
        }
    }

    printf("total with perimeter is %ld\ntotal with sides is %ld\n",
           perimeterTotal, sideTotal);

}

/*
 * Get the number of sides of the shape with the given id. The shape is bounded
 * by the minimum and maximum row and column values specified.
 */
long getSides(int minRow, int maxRow, int minCol, int maxCol, int id)
{
    /* Count the horizontal sides */
    bool contiguousUp = false;
    bool contiguousDown = false;
    long numSides = 0;
    for (int row = minRow; row <= maxRow; row++)
    {
        contiguousUp = false;
        contiguousDown = false;
        for (int col = minCol; col <= maxCol; col++)
        {
            if (!contiguousUp && PlotData[row][col] != NULL &&
                PlotData[row][col]->regionId == id &&
                (!isInBounds(row - 1, col) ||
                 PlotData[row - 1][col] == NULL ||
                 PlotData[row - 1][col]->regionId != id))
            {
                numSides++;
                contiguousUp = true;
            }
            else if (PlotData[row][col] == NULL ||
                     PlotData[row][col]->regionId != id ||
                     (isInBounds(row - 1, col) &&
                      PlotData[row - 1][col] != NULL &&
                      PlotData[row - 1][col]->regionId == id))
            {
                contiguousUp = false;
            }

            if (!contiguousDown && PlotData[row][col] != NULL &&
                PlotData[row][col]->regionId == id &&
                (!isInBounds(row + 1, col) || 
                 PlotData[row + 1][col] == NULL ||
                 PlotData[row + 1][col]->regionId != id))
            {
                numSides++;
                contiguousDown = true;
            }
            else if (PlotData[row][col] == NULL ||
                     PlotData[row][col]->regionId != id ||
                     (isInBounds(row + 1, col) &&
                      PlotData[row + 1][col] != NULL &&
                      PlotData[row + 1][col]->regionId == id))
            {
                contiguousDown = false;
            }
        }
    }

    bool contiguousLeft = false;
    bool contiguousRight = false;
    /* Count the vertical sides*/
    for (int col = minCol; col <= maxCol; col++)
    {
        contiguousRight = false;
        contiguousLeft = false;
        for (int row = minRow; row <= maxRow; row++)
        {
            if (!contiguousLeft && PlotData[row][col] != NULL &&
                PlotData[row][col]->regionId == id &&
                (!isInBounds(row, col - 1) ||
                 PlotData[row][col - 1] == NULL ||
                 PlotData[row][col - 1]->regionId != id))
            {
                numSides++;
                contiguousLeft = true;
            }
            else if (PlotData[row][col] == NULL ||
                     PlotData[row][col]->regionId != id ||
                     (isInBounds(row, col - 1) &&
                      PlotData[row][col - 1] != NULL &&
                      PlotData[row][col - 1]->regionId == id))
            {
                contiguousLeft = false;
            }

            if (!contiguousRight && PlotData[row][col] != NULL &&
                PlotData[row][col]->regionId == id &&
                (!isInBounds(row, col + 1) ||
                 PlotData[row][col + 1] == NULL ||
                 PlotData[row][col + 1]->regionId != id))
            {
                numSides++;
                contiguousRight = true;
            }
            else if (PlotData[row][col] == NULL ||
                     PlotData[row][col]->regionId != id ||
                     (isInBounds(row, col + 1) &&
                      PlotData[row][col + 1] != NULL &&
                      PlotData[row][col + 1]->regionId == id))
            {
                contiguousRight = false;
            }
        }
    }
    return numSides;
}

/*
 * Get the area and perimeter of a region containing the given coordinate.
 * Assign this region a unique ID so that we may distinguish it from other
 * regions when we count the number of sides.
 */
plotData_t processRegion(int row, int col)
{
    plotData_t *newPlotData = malloc (sizeof(plotData_t));
    newPlotData->area = 0;
    newPlotData->regionId = RegionIdCtr;
    newPlotData->minRow = row;
    newPlotData->minCol = col;
    newPlotData->maxRow = row;
    newPlotData->maxCol = col;
    RegionIdCtr++;
    processRegionKernel(row, col, PlotGrid[row][col], newPlotData);
    return *newPlotData;
}

/*
 * Given a coordinate, do a depth first search to get the area and perimeter of
 * the connected component. Mark cells as visited by making the letter within
 * them lowercase.
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
        if (row < thisPlot->minRow)
        {
            thisPlot->minRow = row;
        }
        else if (row > thisPlot->maxRow)
        {
            thisPlot->maxRow = row;
        }

        if (col < thisPlot->minCol)
        {
            thisPlot->minCol = col;
        }
        else if (col > thisPlot->maxCol)
        {
            thisPlot->maxCol = col;
        }

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