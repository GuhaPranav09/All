#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PRODS 20
#define MAX_TERMS 20
#define MAX_NT 20
#define MAX_STATES 50
#define MAX_INPUT 50
#define MAX_ACTION_LEN 10
typedef struct
{
    char lhs;
    char rhs[50];
} Production;
Production prods[MAX_PRODS];
int nProds;
char terminals[MAX_TERMS];
int nTerms;
char nonTerminals[MAX_NT];
int nNonTerms;
int nStates;
char action[MAX_STATES][MAX_TERMS + 1][MAX_ACTION_LEN];
int gotoTable[MAX_STATES][MAX_NT];
int termIndex(char c)
{
    if (c == '$')
        return nTerms;
    for (int i = 0; i < nTerms; i++)
        if (terminals[i] == c)
            return i;
    return -1;
}
int nonTermIndex(char c)
{
    for (int i = 0; i < nNonTerms; i++)
        if (nonTerminals[i] == c)
            return i;
    return -1;
}
void parseInputString(char *input)
{
    int stateStack[MAX_INPUT];
    char symbolStack[MAX_INPUT];
    int top = 0;
    int inputPtr = 0;
    int inputLen = strlen(input);
    stateStack[0] = 0;
    symbolStack[0] = '$';
    printf("\n=== LALR PARSING ===\n");
    printf("%-20s %-20s %-30s\n", "Stack", "Input", "Action");
    printf("----------------------------------------------------------------\n");
    while (1)
    {
        int currentState = stateStack[top];
        char currentInput = (inputPtr < inputLen) ? input[inputPtr] : '$';
        int termIdx = termIndex(currentInput);
        if (termIdx == -1)
        {
            printf("Error: Invalid input symbol '%c'\n", currentInput);
            return;
        }
        char stackStr[100] = "";
        for (int i = 0; i <= top; i++)
        {
            char buf[10];
            sprintf(buf, "%d", stateStack[i]);
            strcat(stackStr, buf);
            if (i < top)
            {
                char symBuf[3];
                sprintf(symBuf, "%c", symbolStack[i + 1]);
                strcat(stackStr, symBuf);
            }
        }
        printf("%-20s %-20s ", stackStr, &input[inputPtr]);
        char *actionStr = action[currentState][termIdx];
        if (strcmp(actionStr, "-") == 0 || strlen(actionStr) == 0)
        {
            printf("%-30s\n", "ERROR - No action defined");
            printf("\nResult: INPUT REJECTED\n");
            return;
        }
        if (actionStr[0] == 's')
        {
            int nextState = atoi(&actionStr[1]);
            printf("%-30s\n", actionStr);
            top++;
            stateStack[top] = nextState;
            symbolStack[top] = currentInput;
            inputPtr++;
        }
        else if (actionStr[0] == 'r')
        {
            int prodNum = atoi(&actionStr[1]);
            if (prodNum < 0 || prodNum >= nProds)
            {
                printf("ERROR - Invalid production number\n");
                return;
            }
            Production prod = prods[prodNum];
            int rhsLen = strlen(prod.rhs);
            char reduceStr[50];
            sprintf(reduceStr, "Reduce by P%d: %c->%s", prodNum, prod.lhs, prod.rhs);
            printf("%-30s\n", reduceStr);
            for (int i = 0; i < rhsLen; i++)
            {
                top--;
            }
            int ntIdx = nonTermIndex(prod.lhs);
            if (ntIdx == -1)
            {
                printf("ERROR - Invalid non-terminal in production\n");
                return;
            }
            int gotoState = gotoTable[stateStack[top]][ntIdx];
            if (gotoState == -1)
            {
                printf("ERROR - No GOTO entry for state %d, symbol %c\n",
                       stateStack[top], prod.lhs);
                return;
            }
            top++;
            stateStack[top] = gotoState;
            symbolStack[top] = prod.lhs;
        }
        else if (strcmp(actionStr, "A") == 0)
        {
            printf("%-30s\n", "ACCEPT");
            printf("\nResult: INPUT ACCEPTED\n");
            return;
        }
        else
        {
            printf("%-30s\n", "ERROR");
            printf("\nResult: INPUT REJECTED\n");
            return;
        }
    }
}
int main()
{
    printf("Enter number of productions: ");
    scanf("%d", &nProds);
    printf("Enter productions (format: A->BC):\n");
    for (int i = 0; i < nProds; i++)
    {
        char buf[50];
        scanf("%s", buf);
        prods[i].lhs = buf[0];
        strcpy(prods[i].rhs, buf + 3);
    }
    printf("\nEnter number of terminals: ");
    scanf("%d", &nTerms);
    for (int i = 0; i < nTerms; i++)
    {
        scanf(" %c", &terminals[i]);
    }
    printf("Enter number of non-terminals: ");
    scanf("%d", &nNonTerms);
    for (int i = 0; i < nNonTerms; i++)
    {
        scanf(" %c", &nonTerminals[i]);
    }
    for (int i = nProds; i > 0; i--)
    {
        prods[i] = prods[i - 1];
    }
    prods[0].lhs = 'S';
    sprintf(prods[0].rhs, "%c", prods[1].lhs);
    nProds++;
    // printf("\nAugmented Grammar:\n");
    // for(int i = 0; i < nProds; i++) {
    //  printf("P%d: %c -> %s\n", i, prods[i].lhs, prods[i].rhs);
    // }
    printf("\nEnter number of LALR states: ");
    scanf("%d", &nStates);
    printf("\nEnter LALR Action Table (rows = states, columns = terminals + $):\n");
    for (int i = 0; i < nStates; i++)
    {
        for (int j = 0; j <= nTerms; j++)
        {
            scanf("%s", action[i][j]);
        }
    }
    printf("\nEnter LALR GOTO Table (rows = states, columns = non-terminals, use - for no transition):\n");
    for (int i = 0; i < nStates; i++)
    {
        for (int j = 0; j < nNonTerms; j++)
        {
            char buf[10];
            scanf("%s", buf);
            if (buf[0] == '-')
                gotoTable[i][j] = -1;
            else
                gotoTable[i][j] = atoi(buf);
        }
    }
    char input[MAX_INPUT];
    printf("\nEnter input string to parse (without $): ");
    scanf("%s", input);
    strcat(input, "$");
    parseInputString(input);
    return 0;
}