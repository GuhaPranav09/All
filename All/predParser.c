#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_RULES 100
#define MAX_SYMBOLS 100
#define MAX_LEN 50
typedef char Token[MAX_LEN];
typedef struct
{
    Token left;
    int rightCount;
    Token right[MAX_SYMBOLS];
} Rule;
Rule grammar[MAX_RULES];
int grammarCount = 0;
Token nonTerminals[MAX_SYMBOLS];
int nonTermCount = 0;
Token terminals[MAX_SYMBOLS];
int termCount = 0;
Token startSym;
typedef struct
{
    Token elements[MAX_SYMBOLS];
    int size;
} TokenSet;
TokenSet firstMap[MAX_SYMBOLS];
TokenSet followMap[MAX_SYMBOLS];
int parseTable[MAX_SYMBOLS][MAX_SYMBOLS];
Token termSymbols[MAX_SYMBOLS];
int termIndexCount = 0;
int contains(Token arr[], int size, const char *tok)
{
    for (int i = 0; i < size; i++)
        if (strcmp(arr[i], tok) == 0)
            return 1;
    return 0;
}
int addSymbol(Token arr[], int *size, const char *tok)
{
    if (!contains(arr, *size, tok))
    {
        strcpy(arr[*size], tok);
        (*size)++;
        return 1;
    }
    return 0;
}
void initSet(TokenSet *s) { s->size = 0; }
int addToSet(TokenSet *s, const char *tok)
{
    if (!contains(s->elements, s->size, tok))
    {
        strcpy(s->elements[s->size++], tok);
        return 1;
    }
    return 0;
}
int findNonTermIndex(const char *nt)
{
    for (int i = 0; i < nonTermCount; i++)
        if (strcmp(nonTerminals[i], nt) == 0)
            return i;
    return -1;
}
int findTermIndex(const char *t)
{
    for (int i = 0; i < termIndexCount; i++)
        if (strcmp(termSymbols[i], t) == 0)
            return i;
    return -1;
}
int splitWithBrackets(const char *line, Token tokens[])
{
    int count = 0, i = 0;
    while (line[i])
    {
        if (isspace(line[i]))
        {
            i++;
            continue;
        }
        if (line[i] == '(' || line[i] == ')' || line[i] == '{' || line[i] == '}' || line[i] == '[' || line[i] == ']')
        {
            tokens[count][0] = line[i];
            tokens[count][1] = '\0';
            count++;
            i++;
        }
        else
        {
            int j = 0;
            while (line[i] && !isspace(line[i]) &&
                   line[i] != '(' && line[i] != ')' && line[i] != '{' && line[i] != '}' && line[i] != '[' && line[i] != ']')
            {
                tokens[count][j++] = line[i++];
            }
            tokens[count][j] = '\0';
            count++;
        }
    }
    return count;
}
void computeFIRST(const char *sym, TokenSet *result)
{
    if (!isupper(sym[0]) || strcmp(sym, "epsilon") == 0)
    {
        addToSet(result, sym);
        return;
    }
    int idx = findNonTermIndex(sym);
    for (int r = 0; r < grammarCount; r++)
    {
        if (strcmp(grammar[r].left, sym) == 0)
        {
            int allEps = 1;
            for (int i = 0; i < grammar[r].rightCount; i++)
            {
                TokenSet temp;
                initSet(&temp);
                computeFIRST(grammar[r].right[i], &temp);
                for (int j = 0; j < temp.size; j++)
                    if (strcmp(temp.elements[j], "epsilon") != 0)
                        addToSet(result, temp.elements[j]);
                if (!contains(temp.elements, temp.size, "epsilon"))
                {
                    allEps = 0;
                    break;
                }
            }
            if (allEps)
                addToSet(result, "epsilon");
        }
    }
}
void computeFOLLOW(const char *sym, TokenSet *result)
{
    if (strcmp(sym, startSym) == 0)
        addToSet(result, "$");
    for (int r = 0; r < grammarCount; r++)
    {
        for (int i = 0; i < grammar[r].rightCount; i++)
        {
            if (strcmp(grammar[r].right[i], sym) == 0)
            {
                int allEps = 1;
                for (int j = i + 1; j < grammar[r].rightCount; j++)
                {
                    TokenSet temp;
                    initSet(&temp);
                    computeFIRST(grammar[r].right[j], &temp);
                    for (int k = 0; k < temp.size; k++)
                        if (strcmp(temp.elements[k], "epsilon") != 0)
                            addToSet(result, temp.elements[k]);
                    if (!contains(temp.elements, temp.size, "epsilon"))
                    {
                        allEps = 0;
                        break;
                    }
                }
                if (i + 1 == grammar[r].rightCount || allEps)
                {
                    if (strcmp(grammar[r].left, sym) != 0)
                    {
                        int lidx = findNonTermIndex(grammar[r].left);
                        computeFOLLOW(grammar[r].left, &followMap[lidx]);
                        for (int k = 0; k < followMap[lidx].size; k++)
                            addToSet(result, followMap[lidx].elements[k]);
                    }
                }
            }
        }
    }
}
void createParseTable()
{
    for (int i = 0; i < nonTermCount; i++)
        for (int j = 0; j < termIndexCount; j++)
            parseTable[i][j] = -1;
    for (int r = 0; r < grammarCount; r++)
    {
        int allEps = 1;
        for (int i = 0; i < grammar[r].rightCount; i++)
        {
            TokenSet temp;
            initSet(&temp);
            computeFIRST(grammar[r].right[i], &temp);
            for (int k = 0; k < temp.size; k++)
            {
                if (strcmp(temp.elements[k], "epsilon") != 0)
                {
                    int nt = findNonTermIndex(grammar[r].left);
                    int t = findTermIndex(temp.elements[k]);
                    parseTable[nt][t] = r;
                }
            }
            if (!contains(temp.elements, temp.size, "epsilon"))
            {
                allEps = 0;
                break;
            }
        }
        if (allEps)
        {
            int nt = findNonTermIndex(grammar[r].left);
            TokenSet f;
            initSet(&f);
            computeFOLLOW(grammar[r].left, &f);
            for (int k = 0; k < f.size; k++)
            {
                int t = findTermIndex(f.elements[k]);
                parseTable[nt][t] = r;
            }
        }
    }
}
void showFirstFollow()
{
    printf("\nFIRST and FOLLOW Sets:\n");
    for (int i = 0; i < nonTermCount; i++)
    {
        initSet(&firstMap[i]);
        computeFIRST(nonTerminals[i], &firstMap[i]);
        printf("FIRST(%s)={", nonTerminals[i]);
        for (int j = 0; j < firstMap[i].size; j++)
            printf("%s ", firstMap[i].elements[j]);
        printf("} ");
        initSet(&followMap[i]);
        computeFOLLOW(nonTerminals[i], &followMap[i]);
        printf("FOLLOW(%s)={", nonTerminals[i]);
        for (int j = 0; j < followMap[i].size; j++)
            printf("%s ", followMap[i].elements[j]);
        printf("}\n");
    }
}
void showParseTable()
{
    printf("\nParse Table:\n ");
    for (int j = 0; j < termIndexCount; j++)
        printf("%8s", termSymbols[j]);
    printf("\n");
    for (int i = 0; i < nonTermCount; i++)
    {
        printf("%-8s", nonTerminals[i]);
        for (int j = 0; j < termIndexCount; j++)
        {
            if (parseTable[i][j] != -1)
            {
                printf("%8s->", grammar[parseTable[i][j]].left);
                for (int k = 0; k < grammar[parseTable[i][j]].rightCount; k++)
                    printf("%s", grammar[parseTable[i][j]].right[k]);
            }
            else
                printf("%8s", "-");
        }
        printf("\n");
    }
}
int main()
{
    int prodCount;
    printf("Enter number of productions: ");
    scanf("%d", &prodCount);
    getchar();
    printf("Enter productions (e.g., E->T E', E'->+ T E', E'->epsilon, T->( E )):\n");
    for (int i = 0; i < prodCount; i++)
    {
        char line[200];
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;
        char *arrow = strstr(line, "->");
        *arrow = '\0';
        char *lhs = line;
        char *rhs = arrow + 2;
        strcpy(grammar[i].left, lhs);
        grammar[i].rightCount = splitWithBrackets(rhs, grammar[i].right);
        grammarCount++;
        addSymbol(nonTerminals, &nonTermCount, lhs);
        for (int j = 0; j < grammar[i].rightCount; j++)
        {
            if (!isupper(grammar[i].right[j][0]) && strcmp(grammar[i].right[j], "epsilon") != 0)
                addSymbol(terminals, &termCount, grammar[i].right[j]);
        }
    }
    strcpy(startSym, grammar[0].left);
    for (int j = 0; j < termCount; j++)
        strcpy(termSymbols[termIndexCount++], terminals[j]);
    strcpy(termSymbols[termIndexCount++], "$");
    showFirstFollow();
    createParseTable();
    showParseTable();
    return 0;
}