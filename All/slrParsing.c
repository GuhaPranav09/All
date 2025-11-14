#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PRODS 50
#define MAX_LEN 100
#define MAX_STATES 50
#define MAX_SYMBOLS 50
typedef struct
{
    char lhs;
    char rhs[MAX_LEN];
} Production;
Production prods[MAX_PRODS];
int nProds;
int nStates, nTerm, nNonTerm;
char actionType[MAX_STATES][MAX_SYMBOLS];
int actionVal[MAX_STATES][MAX_SYMBOLS];
int goToTable[MAX_STATES][MAX_SYMBOLS];
char terminals[MAX_SYMBOLS];
char nonTerminals[MAX_SYMBOLS];
int termIndex(char c)
{
    for (int i = 0; i < nTerm; i++)
        if (terminals[i] == c)
            return i;
    if (c == '$')
        return nTerm;
    return -1;
}
int nonTermIndex(char c)
{
    for (int i = 0; i < nNonTerm; i++)
        if (nonTerminals[i] == c)
            return i;
    return -1;
}
void parseString(char *input)
{
    int stack[100];
    int top = 0;
    stack[top] = 0;
    int ip = 0;
    printf("\n%-20s %-20s %-20s\n", "Stack", "Input", "Action");
    while (1)
    {
        char stkStr[100] = "";
        for (int i = 0; i <= top; i++)
        {
            char tmp[5];
            sprintf(tmp, "%d ", stack[i]);
            strcat(stkStr, tmp);
        }
        char *inpStr = input + ip;
        char a = input[ip];
        int col = termIndex(a);
        if (col == -1)
        {
            printf("Invalid symbol %c\n", a);
            return;
        }
        int state = stack[top];
        char act = actionType[state][col];
        int val = actionVal[state][col];
        char actionDesc[50];
        if (act == 's')
            sprintf(actionDesc, "Shift %d", val);
        else if (act == 'r')
            sprintf(actionDesc, "Reduce by %c->%s", prods[val - 1].lhs, prods[val - 1].rhs);
        else if (act == 'A')
            sprintf(actionDesc, "ACCEPT");
        else
            sprintf(actionDesc, "ERROR");
        printf("%-20s %-20s %-20s\n", stkStr, inpStr, actionDesc);
        if (act == 's')
        {
            top++;
            stack[top] = val;
            ip++;
        }
        else if (act == 'r')
        {
            Production p = prods[val - 1];
            int len = strlen(p.rhs);
            if (p.rhs[0] == '#')
                len = 0;
            top -= len;
            if (top < 0)
            {
                printf("Stack underflow\n");
                return;
            }
            int st = stack[top];
            int A = nonTermIndex(p.lhs);
            if (A == -1 || goToTable[st][A] == -1)
            {
                printf("Invalid GOTO\n");
                return;
            }
            top++;
            stack[top] = goToTable[st][A];
        }
        else if (act == 'A')
            break;
        else
            break;
    }
}
int main()
{
    printf("Enter number of productions: ");
    scanf("%d", &nProds);
    for (int i = 0; i < nProds; i++)
    {
        char buf[MAX_LEN];
        scanf("%s", buf);
        prods[i].lhs = buf[0];
        strcpy(prods[i].rhs, buf + 3);
        int len = strlen(prods[i].rhs);
        if (prods[i].rhs[len - 1] == '\r' || prods[i].rhs[len - 1] == '\n')
            prods[i].rhs[len - 1] = '\0';
    }
    printf("Enter number of terminals (without $): ");
    scanf("%d", &nTerm);
    for (int i = 0; i < nTerm; i++)
    {
        char tmp;
        scanf(" %c", &tmp);
        terminals[i] = tmp;
    }
    terminals[nTerm] = '$';
    printf("Enter number of nonterminals: ");
    scanf("%d", &nNonTerm);
    for (int i = 0; i < nNonTerm; i++)
    {
        char tmp;
        scanf(" %c", &tmp);
        nonTerminals[i] = tmp;
    }
    printf("Enter number of states: ");
    scanf("%d", &nStates);
    printf("Enter ACTION table (rows=states, cols=terminals+$):\n");
    for (int i = 0; i < nStates; i++)
    {
        for (int j = 0; j <= nTerm; j++)
        {
            char buf[10];
            scanf("%s", buf);
            if (buf[0] == 's')
            {
                actionType[i][j] = 's';
                actionVal[i][j] = atoi(buf + 1);
            }
            else if (buf[0] == 'r')
            {
                actionType[i][j] = 'r';
                actionVal[i][j] = atoi(buf + 1);
            }
            else if (buf[0] == 'A')
            {
                actionType[i][j] = 'A';
            }
            else
            {
                actionType[i][j] = '-';
            }
        }
    }
    printf("Enter GOTO table (rows=states, cols=nonterminals), use '-' for empty:\n");
    for (int i = 0; i < nStates; i++)
    {
        for (int j = 0; j < nNonTerm; j++)
        {
            char buf[10];
            scanf("%s", buf);
            if (buf[0] == '-')
                goToTable[i][j] = -1;
            else
                goToTable[i][j] = atoi(buf);
        }
    }
    char inp[MAX_LEN];
    printf("Enter input string (end with $): ");
    scanf("%s", inp);
    parseString(inp);
    return 0;
}