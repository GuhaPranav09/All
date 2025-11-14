#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_RULES 20
#define MAX_LEN 30
#define MAX_STATES 50
#define MAX_ITEMS 100
#define MAX_SYMBOLS 20
typedef struct
{
    char lhs;
    char rhs[MAX_LEN];
} Production;
typedef struct
{
    int prodIndex;
    int dotPos;
} Item;
typedef struct
{
    Item items[MAX_ITEMS];
    int count;
} ItemSet;
Production prods[MAX_RULES];
int nProds;
char nonTerminals[MAX_SYMBOLS];
char terminals[MAX_SYMBOLS];
int nNonTerms = 0, nTerms = 0;
ItemSet states[MAX_STATES];
int nStates = 0;
int transitions[MAX_STATES][MAX_SYMBOLS];
char firstSets[MAX_SYMBOLS][MAX_SYMBOLS];
char followSets[MAX_SYMBOLS][MAX_SYMBOLS];
int isNonTerminal(char c)
{
    return isupper(c);
}
int symbolIndex(char c)
{
    for (int i = 0; i < nNonTerms + nTerms; i++)
    {
        if (i < nNonTerms && nonTerminals[i] == c)
            return i;
        if (i >= nNonTerms && terminals[i - nNonTerms] == c)
            return i;
    }
    return -1;
}
void addSymbol(char *set, char c)
{
    for (int i = 0; set[i]; i++)
        if (set[i] == c)
            return;
    int len = strlen(set);
    set[len] = c;
    set[len + 1] = '\0';
}
void computeFirst()
{
    for (int i = 0; i < nNonTerms; i++)
        firstSets[i][0] = '\0';
    for (int i = 0; i < nNonTerms; i++)
    {
        for (int j = 0; j < nProds; j++)
        {
            if (prods[j].lhs == nonTerminals[i])
            {
                char c = prods[j].rhs[0];
                if (isNonTerminal(c))
                {
                    addSymbol(firstSets[i], firstSets[symbolIndex(c)][0]);
                }
                else
                {
                    addSymbol(firstSets[i], c);
                }
            }
        }
    }
}
void computeFollow()
{
    for (int i = 0; i < nNonTerms; i++)
        followSets[i][0] = '\0';
    addSymbol(followSets[0], '$');
    for (int j = 0; j < nProds; j++)
    {
        char *rhs = prods[j].rhs;
        int len = strlen(rhs);
        for (int k = 0; k < len; k++)
        {
            if (isNonTerminal(rhs[k]))
            {
                if (k + 1 < len)
                {
                    if (isNonTerminal(rhs[k + 1]))
                    {
                        int idx = symbolIndex(rhs[k]);
                        int idx2 = symbolIndex(rhs[k + 1]);
                        for (int x = 0; followSets[idx2][x]; x++)
                            addSymbol(followSets[idx], followSets[idx2][x]);
                    }
                    else
                    {
                        addSymbol(followSets[symbolIndex(rhs[k])], rhs[k + 1]);
                    }
                }
                else
                {
                    int idx = symbolIndex(rhs[k]);
                    for (int x = 0; followSets[symbolIndex(prods[j].lhs)][x]; x++)
                        addSymbol(followSets[idx], followSets[symbolIndex(prods[j].lhs)][x]);
                }
            }
        }
    }
}
ItemSet closure(ItemSet I)
{
    ItemSet J = I;
    int changed = 1;
    while (changed)
    {
        changed = 0;
        for (int i = 0; i < J.count; i++)
        {
            Item it = J.items[i];
            char *rhs = prods[it.prodIndex].rhs;
            if (it.dotPos < strlen(rhs))
            {
                char B = rhs[it.dotPos];
                if (isNonTerminal(B))
                {
                    for (int p = 0; p < nProds; p++)
                    {
                        if (prods[p].lhs == B)
                        {
                            int exists = 0;
                            for (int q = 0; q < J.count; q++)
                            {
                                if (J.items[q].prodIndex == p && J.items[q].dotPos == 0)
                                {
                                    exists = 1;
                                    break;
                                }
                            }
                            if (!exists)
                            {
                                J.items[J.count].prodIndex = p;
                                J.items[J.count].dotPos = 0;
                                J.count++;
                                changed = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return J;
}
ItemSet goTo(ItemSet I, char X)
{
    ItemSet J;
    J.count = 0;
    for (int i = 0; i < I.count; i++)
    {
        Item it = I.items[i];
        char *rhs = prods[it.prodIndex].rhs;
        if (it.dotPos < strlen(rhs) && rhs[it.dotPos] == X)
        {
            J.items[J.count].prodIndex = it.prodIndex;
            J.items[J.count].dotPos = it.dotPos + 1;
            J.count++;
        }
    }
    return closure(J);
}
int equalSets(ItemSet A, ItemSet B)
{
    if (A.count != B.count)
        return 0;
    for (int i = 0; i < A.count; i++)
    {
        int found = 0;
        for (int j = 0; j < B.count; j++)
        {
            if (A.items[i].prodIndex == B.items[j].prodIndex && A.items[i].dotPos == B.items[j].dotPos)
            {
                found = 1;
                break;
            }
        }
        if (!found)
            return 0;
    }
    return 1;
}
void buildStates()
{
    ItemSet I0;
    I0.count = 1;
    I0.items[0].prodIndex = 0;
    I0.items[0].dotPos = 0;
    I0 = closure(I0);
    states[0] = I0;
    nStates = 1;
    for (int i = 0; i < nStates; i++)
    {
        for (int s = 0; s < nNonTerms + nTerms; s++)
        {
            char X = (s < nNonTerms) ? nonTerminals[s] : terminals[s - nNonTerms];
            ItemSet J = goTo(states[i], X);
            if (J.count == 0)
                continue;
            int exists = -1;
            for (int k = 0; k < nStates; k++)
            {
                if (equalSets(states[k], J))
                {
                    exists = k;
                    break;
                }
            }
            if (exists == -1)
            {
                states[nStates] = J;
                transitions[i][s] = nStates;
                nStates++;
            }
            else
            {
                transitions[i][s] = exists;
            }
        }
    }
}
void printStates()
{
    for (int i = 0; i < nStates; i++)
    {
        printf("\nI%d:\n", i);
        for (int j = 0; j < states[i].count; j++)
        {
            Item it = states[i].items[j];
            char *rhs = prods[it.prodIndex].rhs;
            printf(" %c -> ", prods[it.prodIndex].lhs);
            for (int k = 0; k < strlen(rhs); k++)
            {
                if (k == it.dotPos)
                    printf(".");
                printf("%c", rhs[k]);
            }
            if (it.dotPos == strlen(rhs))
                printf(".");
            printf("\n");
        }
    }
}
void buildParsingTable()
{
    printf("\nSLR Parsing Table:\n");
    printf("S\t");
    for (int i = 0; i < nTerms; i++)
        printf("%c\t", terminals[i]);
    printf("$\t");
    for (int i = 0; i < nNonTerms; i++)
        printf("%c\t", nonTerminals[i]);
    printf("\n");
    for (int i = 0; i < nStates; i++)
    {
        printf("%d\t", i);
        for (int t = 0; t < nTerms + 1; t++)
        {
            char a = (t < nTerms) ? terminals[t] : '$';
            char entry[10] = "-";
            for (int j = 0; j < states[i].count; j++)
            {
                Item it = states[i].items[j];
                char *rhs = prods[it.prodIndex].rhs;
                if (it.dotPos < strlen(rhs) && rhs[it.dotPos] == a)
                {
                    sprintf(entry, "s%d", transitions[i][symbolIndex(a)]);
                }
                else if (it.dotPos == strlen(rhs))
                {
                    if (prods[it.prodIndex].lhs == 'S' && it.prodIndex == 0 &&
                        a == '$')
                    {
                        strcpy(entry, "A");
                    }
                    else
                    {
                        int lhsIdx = symbolIndex(prods[it.prodIndex].lhs);
                        for (int f = 0; followSets[lhsIdx][f]; f++)
                        {
                            if (followSets[lhsIdx][f] == a)
                                sprintf(entry, "r%d", it.prodIndex);
                        }
                    }
                }
            }
            printf("%s\t", entry);
        }
        for (int nt = 0; nt < nNonTerms; nt++)
        {
            if (transitions[i][nt] != 0)
                printf("%d\t", transitions[i][nt]);
            else
                printf("-\t");
        }
        printf("\n");
    }
}
int main()
{
    printf("Enter number of productions: ");
    scanf("%d", &nProds);
    printf("Enter productions (e.g., E->E+T):\n");
    for (int i = 0; i < nProds; i++)
    {
        char input[50];
        scanf("%s", input);
        prods[i].lhs = input[0];
        strcpy(prods[i].rhs, input + 3);
        if (strchr(nonTerminals, prods[i].lhs) == NULL)
        {
            nonTerminals[nNonTerms++] = prods[i].lhs;
        }
        for (int j = 0; j < strlen(prods[i].rhs); j++)
        {
            if (!isNonTerminal(prods[i].rhs[j]) && strchr(terminals, prods[i].rhs[j]) == NULL)
            {
                terminals[nTerms++] = prods[i].rhs[j];
            }
        }
    }
    for (int i = nProds; i > 0; i--)
        prods[i] = prods[i - 1];
    prods[0].lhs = 'S';
    sprintf(prods[0].rhs, "%c", prods[1].lhs);
    nProds++;
    computeFirst();
    computeFollow();
    buildStates();
    printStates();
    buildParsingTable();
    return 0;
}