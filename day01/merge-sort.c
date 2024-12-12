#include <stdio.h>
#include <string.h>

#define LIST_LEN 9

void MergeSort (int inputList[], int len);
void mergeSortKernel (int inputList[], int workList[], int startIdx, int endIdx);
void mergeLists (int inputList[], int sortedList[], int startIdx, int middleIdx, int endIdx);

void MergeSort (int inputList[], int len)
{
    int workList[len];
    memcpy(workList, inputList, sizeof(int) * len);
    mergeSortKernel (inputList, workList, 0, len-1);

    memcpy (inputList, workList, sizeof(int) * len);
}

void mergeSortKernel (int inputList[], int workList[], int startIdx, int endIdx)
{
    int middleIdx = 0;
    if (startIdx < endIdx)
    {
        middleIdx = (endIdx + startIdx) / 2;
        mergeSortKernel (workList, inputList, startIdx, middleIdx);
        mergeSortKernel (workList, inputList, middleIdx + 1, endIdx);

        mergeLists (inputList, workList, startIdx, middleIdx, endIdx);
    }
}

void mergeLists (int inputList[], int sortedList[], int startIdx, int middleIdx, int endIdx)
{
    int leftListIdx = startIdx;
    int rightListIdx = middleIdx+1;

    for (int i = startIdx; i <= endIdx; i++)
    {
        if (leftListIdx <= middleIdx && (inputList[leftListIdx] <= inputList[rightListIdx] || rightListIdx > endIdx))
        {
            /* Element from the left list is smaller; put that in the sorted list */
            sortedList[i] = inputList[leftListIdx];
            leftListIdx++;
        }
        else
        {
            /* Element from the right list is smaller; put that in the sorted list */
            sortedList[i] = inputList[rightListIdx];
            rightListIdx++;
        }
    }
}