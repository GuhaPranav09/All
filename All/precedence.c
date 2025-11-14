#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAX 20
#define SIZE 100
char productions[MAX][SIZE];
int n;
char nonTerminals[MAX];
int ntCount = 0;
char terminals[MAX];
int termCount = 0;
int leading[MAX][MAX];
int trailing[MAX][MAX];
int opPrecedenceTable[MAX][MAX];
int findNonTerminalIndex(char c)
{
    for (int i = 0; i < ntCount; i++)
    {
        if (nonTerminals[i] == c)
            return i;
    }
    return -1;
}
int findTerminalIndex(char c)
{
    for (int i = 0; i < termCount; i++)
    {
        if (terminals[i] == c)
            return i;
    }
    return -1;
}
int isNonTerminal(char c)
{
    return (c >= 'A' && c <= 'Z');
}
void addTerminal(char c)
{
    if (c == '#' || c == '\0')
        return;
    if (!isNonTerminal(c) && strchr(terminals, c) == NULL)
    {
        terminals[termCount++] = c;
    }
}
void collectNonTerminals()
{
    for (int i = 0; i < n; i++)
    {
        if (!strchr(nonTerminals, productions[i][0]))
        {
            nonTerminals[ntCount++] = productions[i][0];
        }
    }
}
void collectTerminals()
{
    for (int i = 0; i < n; i++)
    {
        char *rhs = productions[i] + 3;
        for (int j = 0; rhs[j] != '\0'; j++)
        {
            addTerminal(rhs[j]);
        }
    }
}
void computeLeading()
{
    for (int i = 0; i < ntCount; i++)
        for (int j = 0; j < termCount; j++)
            leading[i][j] = 0;
    int changed;
    do
    {
        changed = 0;
        for (int i = 0; i < n; i++)
        {
            int A = findNonTerminalIndex(productions[i][0]);
            char *rhs = productions[i] + 3;
            if (rhs[0] == '\0')
                continue;
            if (!isNonTerminal(rhs[0]))
            {
                int t = findTerminalIndex(rhs[0]);
                if (t != -1 && leading[A][t] == 0)
                {
                    leading[A][t] = 1;
                    changed = 1;
                }
            }
            else if (rhs[1] != '\0' && !isNonTerminal(rhs[1]))
            {
                int t = findTerminalIndex(rhs[1]);
                if (t != -1 && leading[A][t] == 0)
                {
                    leading[A][t] = 1;
                    changed = 1;
                }
            }
            if (isNonTerminal(rhs[0]))
            {
                int B = findNonTerminalIndex(rhs[0]);
                if (B != -1)
                {
                    for (int k = 0; k < termCount; k++)
                    {
                        if (leading[B][k] == 1 && leading[A][k] == 0)
                        {
                            leading[A][k] = 1;
                            changed = 1;
                        }
                    }
                }
            }
        }
    } while (changed);
}
void computeTrailing()
{
    for (int i = 0; i < ntCount; i++)
        for (int j = 0; j < termCount; j++)
            trailing[i][j] = 0;
    int changed;
    do
    {
        changed = 0;
        for (int i = 0; i < n; i++)
        {
            int A = findNonTerminalIndex(productions[i][0]);
            char *rhs = productions[i] + 3;
            int len = strlen(rhs);
            if (len == 0)
                continue;
            if (!isNonTerminal(rhs[len - 1]))
            {
                int t = findTerminalIndex(rhs[len - 1]);
                if (t != -1 && trailing[A][t] == 0)
                {
                    trailing[A][t] = 1;
                    changed = 1;
                }
            }
            else if (len > 1 && !isNonTerminal(rhs[len - 2]))
            {
                int t = findTerminalIndex(rhs[len - 2]);
                if (t != -1 && trailing[A][t] == 0)
                {
                    trailing[A][t] = 1;
                    changed = 1;
                }
            }
            if (isNonTerminal(rhs[len - 1]))
            {
                int B = findNonTerminalIndex(rhs[len - 1]);
                if (B != -1)
                {
                    for (int k = 0; k < termCount; k++)
                    {
                        if (trailing[B][k] == 1 && trailing[A][k] == 0)
                        {
                            trailing[A][k] = 1;
                            changed = 1;
                        }
                    }
                }
            }
        }
    } while (changed);
}
void buildOperatorPrecedenceTable()
{
    for (int i = 0; i < termCount; i++)
        for (int j = 0; j < termCount; j++)
            opPrecedenceTable[i][j] = 0;
    for (int i = 0; i < n; i++)
    {
        char *rhs = productions[i] + 3;
        int len = strlen(rhs);
        for (int k = 0; k < len - 1; k++)
        {
            char a = rhs[k];
            char b = rhs[k + 1];
            if (!isNonTerminal(a) && !isNonTerminal(b))
            {
                int row = findTerminalIndex(a);
                int col = findTerminalIndex(b);
                if (row != -1 && col != -1)
                    opPrecedenceTable[row][col] = 2; // =
            }
            if (!isNonTerminal(a) && isNonTerminal(b))
            {
                int row = findTerminalIndex(a);
                int B = findNonTerminalIndex(b);
                if (row != -1 && B != -1)
                {
                    for (int t = 0; t < termCount; t++)
                        if (leading[B][t])
                            opPrecedenceTable[row][t] = 1; // <
                }
            }
            if (isNonTerminal(a) && !isNonTerminal(b))
            {
                int A = findNonTerminalIndex(a);
                int col = findTerminalIndex(b);
                if (A != -1 && col != -1)
                {
                    for (int t = 0; t < termCount; t++)
                        if (trailing[A][t])
                            opPrecedenceTable[t][col] = 3; // >
                }
            }
            if (k < len - 2 && !isNonTerminal(rhs[k]) &&
                isNonTerminal(rhs[k + 1]) && !isNonTerminal(rhs[k + 2]))
            {
                int row = findTerminalIndex(rhs[k]);     // left terminal
                int col = findTerminalIndex(rhs[k + 2]); // right terminal
                if (row != -1 && col != -1)
                    opPrecedenceTable[row][col] = 2; // =
            }
        }
    }
    if (!strchr(terminals, '$'))
        terminals[termCount++] = '$';
    int dollarIndex = findTerminalIndex('$');
    for (int i = 0; i < termCount; i++)
    {
        if (i == dollarIndex)
            continue;
        opPrecedenceTable[dollarIndex][i] = 1; // $ <
        opPrecedenceTable[i][dollarIndex] = 3; // > $
    }
}
void printSet(const char *setName, int sets[MAX][MAX], int idx)
{
    printf("%s(%c) = { ", setName, nonTerminals[idx]);
    for (int i = 0; i < termCount; i++)
    {
        if (sets[idx][i])
            printf("%c ", terminals[i]);
    }
    printf("}\n");
}
void printOperatorPrecedenceTable()
{
    printf("\nOperator Precedence Table:\n ");
    for (int i = 0; i < termCount; i++)
    {
        printf("%2c ", terminals[i]);
    }
    printf("\n");
    for (int i = 0; i < termCount; i++)
    {
        printf("%2c ", terminals[i]);
        for (int j = 0; j < termCount; j++)
        {
            char *rel = " ";
            switch (opPrecedenceTable[i][j])
            {
            case 1:
                rel = "<";
                break;
            case 2:
                rel = "=";
                break;
            case 3:
                rel = ">";
                break;
            }
            printf(" %s ", rel);
        }
        printf("\n");
    }
}
int main()
{
    printf("Enter number of productions: ");
    scanf("%d", &n);
    getchar();
    printf("Enter productions (like E->E+T or E->T):\n");
    for (int i = 0; i < n; i++)
    {
        fgets(productions[i], SIZE, stdin);
        productions[i][strcspn(productions[i], "\n")] = '\0';
    }
    collectNonTerminals();
    collectTerminals();
    printf("\nNonTerminals: ");
    for (int i = 0; i < ntCount; i++)
        printf("%c ", nonTerminals[i]);
    printf("\nTerminals: ");
    for (int i = 0; i < termCount; i++)
        printf("%c ", terminals[i]);
    printf("\n");
    computeLeading();
    computeTrailing();
    printf("\nLeading sets:\n");
    for (int i = 0; i < ntCount; i++)
        printSet("Leading", leading, i);
    printf("\nTrailing sets:\n");
    for (int i = 0; i < ntCount; i++)
        printSet("Trailing", trailing, i);
    buildOperatorPrecedenceTable();
    printOperatorPrecedenceTable();
    return 0;
}