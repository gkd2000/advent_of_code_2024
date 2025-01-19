#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define NAME_LEN 3
#define NUM_ZS 45
#define HASH_MAP_LEN 36 * 36 * 36

typedef enum
{
    AND,
    OR,
    XOR,
    NONE
} operation_t;

typedef struct node_s
{
    /* For viewing the nodes as elements of trees */
    struct node_s *parent;
    struct node_s *left;
    struct node_s *right;
    /* For viewing the nodes as elements of a linked list */
    // struct node_s *previous;
    // struct node_s *next;
    char name[4];
    char val;
    bool linked;
    operation_t operation; /* Tells you how to combine this node with its sibling to obtain the parent's value */
} node_t;

char getStartingInput(char *target, FILE *filePointer);
void printTree(node_t *node, int depth);
char evaluateTarget(char *target, FILE *filePointer, long offset, node_t *node);
void connectTrees(node_t *node);
void freeSubtree(node_t *node);
void findBadRules(FILE *filePointer, long offset);
void findOtherBadRules();

char Rules[250][3 + 1 + 3 + 3];
int NumRules = 0;

node_t *Trees[NUM_ZS + 1];

int main (int argc, char **argv)
{
    /* Validate argument */
    // if (argc < 2)
    // {
    //     printf("Please provide an input file\n");
    //     return 0;
    // }

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
    size_t maxLen = 0;
    int lineLen = 0;
    int currentLine = 0;
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1 &&
            line[0] != '\n')
    {
        /* do nothing */
    }

    /*
     * Position in file is now at blank line which separates starting values
     * from relationships. Advance it one more, then save the position so that
     * we can seek back to here as needed.
     */
    // getline(&line, &maxLen, filePointer);
    long startOfOperations = ftell(filePointer);

    /* Get the number of lines remaining in the file */
    int numLines = 0;
    while(getline(&line, &maxLen, filePointer) != -1)
    {
        numLines++;
    }

    char target[4] = "z00";
    uint64_t total = 0;
    uint64_t temp = 0;
    node_t *curNode = NULL;
    // node_t *trees[NUM_ZS+1];
    for (int i = 0; i <= NUM_ZS; i++)
    {
        curNode = malloc(sizeof(node_t));
        Trees[i] = curNode;
        curNode->operation = NONE;
        temp = evaluateTarget(target, filePointer, startOfOperations, curNode);
        temp = temp << i;
        total += temp;
        // printf("%s\n", target);
        if (target[2] == '9')
        {
            target[1] += 1;
            target[2] = '0';
        }
        else
        {
            target[2] = (char) (target[2] + 1);
        }
    }

    // for (int i = 0; i <= NUM_ZS; i++)
    // {
    //     printTree(Trees[i], 0);
    //     printf("\n");
    // }

    findBadRules(filePointer, startOfOperations);

    // for (int i = 0; i < NumRules; i++)
    // {
    //     printf("%.3s %c %.3s -> %.3s\n", &Rules[i][0], Rules[i][3],
    //            &Rules[i][4], &Rules[i][7]);
    // }

    findOtherBadRules();

    // connectTrees(Trees[NUM_ZS]);

    // for (int i = 0; i <= NUM_ZS; i++)
    // {
    //     printTree(Trees[i], 0);
    //     printf("\n");
    // }

    uint64_t x = 0;
    uint64_t y = 0;
    fseek(filePointer, 0, SEEK_SET);
    int shiftAmt = 0;
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1 &&
            line[0] == 'x')
    {
        temp = ((uint64_t) (line[5] - '0')) << shiftAmt;
        // printf("shiftAmt: %d, number is %d, adding %llu\n", shiftAmt, (line[5] - '0'), temp);
        x += temp;
        shiftAmt++;
    }
    shiftAmt = 0;
    // printf("\n");
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1 &&
            line[0] == 'y')
    {
        temp = ((uint64_t) (line[5] - '0')) << shiftAmt;
        y += temp;
        // printf("adding %llu\n", temp);
        shiftAmt++;
    }

    // char *target = "z00";
    // char ans = evaluateTarget(target, filePointer, startOfOperations);
    printf("%llu\n", total);
    printf("initial x: %llu, initial y: %llu, target: %llu\n", x, y, x + y);
    printf("difference is %llX\n", total ^ (x + y));
}

void findOtherBadRules()
{
    for (int i = 0; i < NumRules; i++)
    {
        if ((Rules[i][0] == 'x' || Rules[i][0] == 'y') && Rules[i][3] == 'X')
        {
            bool foundGate = false;
            char *target = &Rules[i][7];
            /* The output gate must be the input for another XOR */
            for (int j = 0; j < NumRules && !foundGate; j++)
            {
                if ((memcmp(&Rules[j][0], target, 3) == 0 ||
                     memcmp(&Rules[j][4], target, 3) == 0) &&
                    Rules[j][3] == 'X')
                {
                    foundGate = true;
                }
            }
            if (!foundGate)
            {
                printf("%.10s\n", &Rules[i][0]);
            }
        }
        else if ((Rules[i][0] == 'x' || Rules[i][0] == 'y') && Rules[i][3] == 'A')
        {
            bool foundGate = false;
            char *target = &Rules[i][7];
            /* The output gate must be the input for an OR */
            for (int j = 0; j < NumRules && !foundGate; j++)
            {
                if ((memcmp(&Rules[j][0], target, 3) == 0 ||
                     memcmp(&Rules[j][4], target, 3) == 0) &&
                    Rules[j][3] == 'O')
                {
                    foundGate = true;
                }
            }
            if (!foundGate)
            {
                printf("%.10s\n", &Rules[i][0]);
            }
        }
    }
}

void findBadRules(FILE *filePointer, long offset)
{
    fseek(filePointer, offset, SEEK_SET);
    bool foundTarget = false;
    char *line = NULL;
    size_t maxLen = 0;
    int lineLen = 0;
    int currentLine = 0;
    char *result;
    while ((lineLen = getline(&line, &maxLen, filePointer)) != -1)
    {
        result = line + (lineLen - 4);
        if (result[0] == 'z' && memcmp(result, "z45", 3) != 0 && line[4] != 'X')
        {
            /*
             * The output is a z gate that isn't the last one, and we are not
             * XORing. This is bad
             */
            printf("%s", line);
        }
        else if (result[0] != 'z' && line[0] != 'x' && line[0] != 'y' && line[4] == 'X')
        {
            /*
             * The output is not a z gate, and the inputs are not x and y gates,
             * but the operation is XOR. This is bad.
             */
            printf("%s", line);
        }
        memcpy(&Rules[NumRules][0], line, 3);
        Rules[NumRules][3] = line[4];
        memcpy(&Rules[NumRules][4], line + lineLen - 11, 3);
        memcpy(&Rules[NumRules][7], result, 3);
        NumRules++;
    }

}

/*
 * Create an array that contains pointers to the nodes. Since we have two nodes
 * for x00 (for example), this will need to be an array of pointers to pointers
 * to nodes. I think that we should have no more than two pointers per list, though.
 * Find the index in the list by essentially writing the name of the node in base
 * 36. Shouldn't have any collisions.
 * Create another array that contains all of the combinations from the file. This
 * info is already encoded in the binary tree, but it might be easier to do it
 * this way. So this array would maybe be a 3 * (however many rules are present)
 * matrix.
 * Iterate through the rules matrix, finding candidates for swaps. 
 * Need to go through every pair of swaps in order to see 
 */

/*
 * Iterate through the final tree in the list. For each node, find out if it is
 * the child of a root of another tree. If it is, then free all the children of
 * the child, make the operation for the root the operation for its children
 * (so that we preserve the knowledge of how to combine these children), and
 * make the node in the final tree also be the child of the root in question.
 */
void connectTrees(node_t *node)
{
    /*
     * When i = 4, getting a seg fault when node is cjv. In Trees[i], it seems
     * like cjv has a left child at address 0x003a, which probably garbage data.
     * Maybe it isn't being initialized properly? It should have a real child,
     * whether I've already seen it or not, since it is not a leaf in any of
     * the trees
     * The segfault is happening when I come across a node that appears twice in
     * the biggest tree. The second time, when I free its subtree, I'm freeing
     * stuff from the big tree. I think we want an indicator in the node
     * structure to see if it's already linked. I think it will work okay.
     */
    if (node == NULL)
    {
        return;
    }
    for (int i = 0; i < NUM_ZS; i++)
    {
        if (memcmp(node->name, Trees[i]->left->name, 3) == 0 &&
            node != Trees[i]->left &&
            !Trees[i]->left->linked)
        {
            Trees[i]->operation = Trees[i]->left->operation;
            freeSubtree(Trees[i]->left);
            Trees[i]->left = node;
            node->linked = true;
        }
        else if (memcmp(node->name, Trees[i]->right->name, 3) == 0 &&
            node != Trees[i]->right &&
            !Trees[i]->right->linked)
        {
            Trees[i]->operation = Trees[i]->right->operation;
            freeSubtree(Trees[i]->right);
            Trees[i]->right = node;
            node->linked = true;
        }
    }
    connectTrees(node->left);
    connectTrees(node->right);
}

void freeSubtree(node_t *node)
{
    if (node != NULL)
    {
        freeSubtree(node->left);
        freeSubtree(node->right);
        free(node);
    }
}

void printTree(node_t *node, int depth)
{
    if (node != NULL)
    {
        for (int i = 0; i < depth; i++)
        {
            printf(" ");
        }
        printf("%s, %p, ", node->name, node);
        switch (node->operation)
        {
        case OR:
            printf("OR\n");
            break;
        case XOR:
            printf("XOR\n");
            break;
        case AND:
            printf("AND\n");
            break;
        case NONE:
            printf("NONE\n");
            break;
        default:
            break;
        }
        printTree(node->left, depth + 1);
        printTree(node->right, depth + 1);
    }
}

char evaluateTarget(char *target, FILE *filePointer, long offset, node_t *node)
{
    /* Populate this node's name */
    memcpy(node->name, target, 4);
    node->linked = false;
    // printf("target: %s\n", target);
    char retVal;
    /* Base case: target is one of the starting inputs */
    if (target[0] == 'x' || target[0] == 'y')
    {
        retVal = getStartingInput(target, filePointer);
        // printf("%s: %d\n", target, retVal);
        node->val = retVal;
        return retVal;
    }
    fseek(filePointer, offset, SEEK_SET);
    bool foundTarget = false;
    char *line = NULL;
    size_t maxLen = 0;
    int lineLen = 0;
    int currentLine = 0;
    while (!foundTarget && (lineLen = getline(&line, &maxLen, filePointer)) != -1)
    {
        /* Check whether the result on this line is our target */
        if (lineLen == 19)
        {
            /* Operation is XOR or AND */
            foundTarget = memcmp(target, line + 15, 3) == 0;
        }
        else
        {
            foundTarget = memcmp(target, line + 14, 3) == 0;
        }
    }
    /* Copy the operands into their own buffers, then combine them as
     * prescribed */
    if (foundTarget)
    {
        char firstOp[4];
        char secondOp[4];
        memcpy(firstOp, line, 3);
        firstOp[3] = '\0';
        if (lineLen == 19)
        {
            memcpy(secondOp, line + 8, 3);
        }
        else
        {
            memcpy(secondOp, line + 7, 3);
        }
        secondOp[3] = '\0';
        node_t *leftNode = malloc(sizeof(node_t));
        leftNode->parent = node;
        node->left = leftNode;
        node_t *rightNode = malloc(sizeof(node_t));
        rightNode->parent = node;
        node->right = rightNode;
        if (memcmp("XOR", line + 4, 3) == 0)
        {
            leftNode->operation = XOR;
            rightNode->operation = XOR;
            retVal = evaluateTarget(firstOp, filePointer, offset, leftNode) ^ evaluateTarget(secondOp, filePointer, offset, rightNode);
            // printf("%s: %d\n", target, retVal);
            node->val = retVal;
            return retVal;
        }
        else if (memcmp("OR", line + 4, 2) == 0)
        {
            leftNode->operation = OR;
            rightNode->operation = OR;
            retVal = evaluateTarget(firstOp, filePointer, offset, leftNode) | evaluateTarget(secondOp, filePointer, offset, rightNode);
            // printf("%s: %d\n", target, retVal);
            node->val = retVal;
            return retVal;
        }
        else
        {
            leftNode->operation = AND;
            rightNode->operation = AND;
            retVal = evaluateTarget(firstOp, filePointer, offset, leftNode) & evaluateTarget(secondOp, filePointer, offset, rightNode);
            // printf("%s: %d\n", target, retVal);
            node->val = retVal;
            return retVal;
        }
    }
    return 0;
}

char getStartingInput(char *target, FILE *filePointer)
{
    char *line = NULL;
    size_t maxLen = 0;

    /* Seek to the beginning of the file */
    fseek(filePointer, 0, SEEK_SET);
    while (getline(&line, &maxLen, filePointer) != -1 &&
           memcmp(line, target, 3) != 0)
    {
        /* Keep reading the file */
    }
    
    /* line should be the one that matches target */
    return (line[5] - '0');
}
