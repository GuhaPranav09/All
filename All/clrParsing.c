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
void parseInput(char *input)
{
    int stack[MAX_INPUT], top = 0;
    char symStack[MAX_INPUT];
    stack[0] = 0;
    symStack[0] = '$';
    int ip = 0;
    int lenInput = strlen(input);
    printf("\n%-15s%-15s%-25s\n", "Stack", "Input", "Action");
    printf("--------------------------------------------------------\n");
    while (1)
    {
        int state = stack[top];
        char curr = (ip < lenInput) ? input[ip] : '$';
        int tIndex = termIndex(curr);
        char stackStr[100] = "";
        for (int i = 0; i <= top; i++)
        {
            char buf[5];
            sprintf(buf, "%d ", stack[i]);
            strcat(stackStr, buf);
        }
        printf("%-15s%-15s", stackStr, &input[ip]);
        if (strcmp(action[state][tIndex], "-") == 0)
        {
            printf("%-25s\n", "Error!");
            return;
        }
        if (action[state][tIndex][0] == 's')
        {
            int s = atoi(&action[state][tIndex][1]);
            printf("%-25s\n", action[state][tIndex]);
            top++;
            stack[top] = s;
            symStack[top] = curr;
            ip++;
        }
        else if (action[state][tIndex][0] == 'r')
        {
            int r = atoi(&action[state][tIndex][1]);
            int len = strlen(prods[r].rhs);
            char actStr[25];
            sprintf(actStr, "Reduce by %c->%s", prods[r].lhs, prods[r].rhs);
            printf("%-25s\n", actStr);
            top -= len;
            char lhs = prods[r].lhs;
            int g = gotoTable[stack[top]][nonTermIndex(lhs)];
            if (g == -1)
            {
                printf("%-25s\n", "Error! No GOTO");
                return;
            }
            top++;
            stack[top] = g;
            symStack[top] = lhs;
        }
        else if (action[state][tIndex][0] == 'A')
        {
            printf("%-25s\n", "Accept");
            return;
        }
    }
}
int main()
{
    printf("Enter number of productions: ");
    scanf("%d", &nProds);
    for (int i = 0; i < nProds; i++)
    {
        char buf[50];
        scanf("%s", buf);
        prods[i].lhs = buf[0];
        strcpy(prods[i].rhs, buf + 3);
    }
    printf("Enter number of terminals: ");
    scanf("%d", &nTerms);
    for (int i = 0; i < nTerms; i++)
        scanf(" %c", &terminals[i]);
    printf("Enter number of non-terminals: ");
    scanf("%d", &nNonTerms);
    for (int i = 0; i < nNonTerms; i++)
        scanf(" %c", &nonTerminals[i]);
    for (int i = nProds; i > 0; i--)
        prods[i] = prods[i - 1];
    prods[0].lhs = 'S';
    sprintf(prods[0].rhs, "%c", prods[1].lhs);
    nProds++;
    int nStates;
    printf("Enter number of states: ");
    scanf("%d", &nStates);
    printf("Enter Action Table (rows = states, columns = terminals + $):\n");
    for (int i = 0; i < nStates; i++)
    {
        for (int j = 0; j <= nTerms; j++)
            scanf("%s", action[i][j]);
    }
    printf("Enter GOTO Table (rows = states, columns = non-terminals, use - for no transition):\n");
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
    char input[50];
    printf("Enter input string: ");
    scanf("%s", input);
    parseInput(input);
    return 0;
}