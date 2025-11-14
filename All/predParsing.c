#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_SYMBOLS 50
#define MAX_RULES 100
#define MAX_TOKENS 100
#define MAX_LEN 50
typedef char Token[MAX_LEN];
typedef struct
{
    Token left;
    int rightCount;
    Token right[MAX_SYMBOLS];
} Rule;
Rule parseTable[MAX_SYMBOLS][MAX_SYMBOLS];
int used[MAX_SYMBOLS][MAX_SYMBOLS];
Token nonTerminals[MAX_SYMBOLS];
int nonTermCount = 0;
Token terminals[MAX_SYMBOLS];
int termCount = 0;
Token startSym;
int findNonTermIndex(const char *nt)
{
    for (int i = 0; i < nonTermCount; i++)
        if (strcmp(nonTerminals[i], nt) == 0)
            return i;
    return -1;
}
int findTermIndex(const char *t)
{
    for (int i = 0; i < termCount; i++)
        if (strcmp(terminals[i], t) == 0)
            return i;
    return -1;
}
typedef struct
{
    Token arr[MAX_TOKENS];
    int top;
} Stack;
void push(Stack *s, const char *tok) { strcpy(s->arr[++s->top], tok); }
void pop(Stack *s)
{
    if (s->top >= 0)
        s->top--;
}
char *peek(Stack *s) { return s->arr[s->top]; }
int split(const char *line, Token tokens[])
{
    int count = 0, i = 0;
    while (line[i])
    {
        if (isspace(line[i]))
        {
            i++;
            continue;
        }
        int j = 0;
        while (line[i] && !isspace(line[i]))
            tokens[count][j++] = line[i++];
        tokens[count][j] = '\0';
        count++;
    }
    return count;
}
int runParser(Token input[], int n)
{
    Stack stk;
    stk.top = -1;
    push(&stk, "$");
    push(&stk, startSym);
    int idx = 0;
    printf("\nParsing steps:\n");
    printf("%-20s %-20s %s\n", "Stack", "Input", "Action");
    while (stk.top >= 0)
    {
        char *top = peek(&stk);
        char *cur = input[idx];
        {
            printf("[");
            for (int i = 0; i <= stk.top; i++)
            {
                printf("%s", stk.arr[i]);
                if (i != stk.top)
                    printf(" ");
            }
            printf("]");
            printf(" [");
            for (int i = idx; i < n; i++)
            {
                printf("%s", input[i]);
                if (i != n - 1)
                    printf(" ");
            }
            printf("] ");
        }
        if (strcmp(top, cur) == 0)
        {
            if (strcmp(top, "$") == 0)
            {
                printf("ACCEPT\n");
                return 1;
            }
            pop(&stk);
            idx++;
            printf("Match %s\n", cur);
        }
        else if (isupper(top[0]))
        {
            int nt = findNonTermIndex(top);
            int t = findTermIndex(cur);
            if (nt < 0 || t < 0 || !used[nt][t])
            {
                printf("\nERROR: No rule for (%s,%s)\n", top, cur);
                return 0;
            }
            Rule r = parseTable[nt][t];
            pop(&stk);
            if (!(r.rightCount == 1 && strcmp(r.right[0], "epsilon") == 0))
            {
                for (int k = r.rightCount - 1; k >= 0; k--)
                    push(&stk, r.right[k]);
            }
            printf("%s -> ", r.left);
            for (int k = 0; k < r.rightCount; k++)
                printf("%s ", r.right[k]);
            printf("\n");
        }
        else
        {
            printf("\nERROR: Terminal mismatch (%s vs %s)\n", top, cur);
            return 0;
        }
    }
    return 0;
}
int main()
{
    printf("Enter number of non-terminals: ");
    scanf("%d", &nonTermCount);
    getchar();
    printf("Enter non-terminals (one per line):\n");
    for (int i = 0; i < nonTermCount; i++)
    {
        fgets(nonTerminals[i], MAX_LEN, stdin);
        nonTerminals[i][strcspn(nonTerminals[i], "\n")] = 0; // remove newline
    }
    printf("Enter number of terminals: ");
    scanf("%d", &termCount);
    getchar();
    printf("Enter terminals (one per line, include $ as end marker):\n");
    for (int i = 0; i < termCount; i++)
    {
        fgets(terminals[i], MAX_LEN, stdin);
        terminals[i][strcspn(terminals[i], "\n")] = 0;
    }
    printf("\nEnter parse table entries (enter # for empty entry):\n");
    for (int i = 0; i < nonTermCount; i++)
    {
        for (int j = 0; j < termCount; j++)
        {
            char line[200];
            printf("Entry for [%s, %s]: ", nonTerminals[i], terminals[j]);
            fgets(line, sizeof(line), stdin);
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, "#") == 0)
            {
                used[i][j] = 0;
                continue;
            }
            used[i][j] = 1;
            char *arrow = strstr(line, "->");
            if (!arrow)
            {
                printf("Invalid format. Use A->... format.\n");
                return 1;
            }
            *arrow = 0;
            strcpy(parseTable[i][j].left, line);
            char *rhs = arrow + 2;
            parseTable[i][j].rightCount = split(rhs, parseTable[i][j].right);
        }
    }
    int startIndex = -1;
    for (int i = 0; i < nonTermCount; i++)
    {
        for (int j = 0; j < termCount; j++)
        {
            if (used[i][j])
            {
                startIndex = i;
                break;
            }
        }
        if (startIndex != -1)
            break;
    }
    if (startIndex == -1)
    {
        printf("Error: No valid parse table entries found.\n");
        return 1;
    }
    strcpy(startSym, nonTerminals[startIndex]);
    printf("\nStart symbol automatically set to: %s\n", startSym);
    while (1)
    {
        char inputLine[200];
        printf("\nEnter input string to parse (tokens separated by space, 0 to exit): ");
        fgets(inputLine, sizeof(inputLine), stdin);
        inputLine[strcspn(inputLine, "\n")] = 0;
        if (strcmp(inputLine, "0") == 0)
            break;
        Token tokens[MAX_TOKENS];
        int count = split(inputLine, tokens);
        strcpy(tokens[count++], "$");
        int ok = runParser(tokens, count);
        printf("\nResult: The string %s accepted by the grammar.\n", ok ? "IS" : "is NOT");
    }
    printf("\nParser terminated. Goodbye!\n");
    return 0;
}