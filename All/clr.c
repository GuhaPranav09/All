#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_PRODS 20
#define MAX_LEN 30
#define MAX_STATES 50
#define MAX_ITEMS 200
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
    char lookahead[MAX_SYMBOLS];
} ItemLR1;
typedef struct
{
    ItemLR1 items[MAX_ITEMS];
    int count;
} ItemSetLR1;
Production prods[MAX_PRODS];
int nProds;
char terminals[MAX_SYMBOLS];
int nTerms = 0;
char nonTerminals[MAX_SYMBOLS];
int nNonTerms = 0;
ItemSetLR1 states[MAX_STATES];
int nStates = 0;
int transitions[MAX_STATES][MAX_SYMBOLS];
int isNonTerminal(char c) { return isupper(c); }
int contains(char *set, char c)
{
    for (int i = 0; i < strlen(set); i++)
        if (set[i] == c)
            return 1;
    return 0;
}
void addSymbol(char *set, char c)
{
    if (!contains(set, c))
    {
        int len = strlen(set);
        set[len] = c;
        set[len + 1] = '\0';
    }
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
char firstSets[MAX_SYMBOLS][MAX_SYMBOLS];
int firstCount[MAX_SYMBOLS];
int idxNonTerm(char c)
{
    for (int i = 0; i < nNonTerms; i++)
        if (nonTerminals[i] == c)
            return i;
    return -1;
}
void computeFirstSets()
{
    for (int i = 0; i < nNonTerms; i++)
    {
        firstSets[i][0] = '\0';
        firstCount[i] = 0;
    }
    int changed = 1;
    while (changed)
    {
        changed = 0;
        for (int i = 0; i < nProds; i++)
        {
            char A = prods[i].lhs;
            char *rhs = prods[i].rhs;
            int idxA = idxNonTerm(A);
            char old[MAX_SYMBOLS];
            strcpy(old, firstSets[idxA]);
            if (isNonTerminal(rhs[0]))
            {
                int idxB = idxNonTerm(rhs[0]);
                for (int k = 0; k < strlen(firstSets[idxB]); k++)
                    addSymbol(firstSets[idxA], firstSets[idxB][k]);
            }
            else
                addSymbol(firstSets[idxA], rhs[0]);
            if (strcmp(old, firstSets[idxA]) != 0)
                changed = 1;
        }
    }
    printf("\n--- First Sets ---\n");
    for (int i = 0; i < nNonTerms; i++)
    {
        printf("First(%c) = { ", nonTerminals[i]);
        for (int j = 0; j < strlen(firstSets[i]); j++)
            printf("%c ", firstSets[i][j]);
        printf("}\n");
    }
}
ItemSetLR1 closure(ItemSetLR1 I)
{
    ItemSetLR1 J = I;
    int changed = 1;
    while (changed)
    {
        changed = 0;
        for (int i = 0; i < J.count; i++)
        {
            ItemLR1 it = J.items[i];
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
                            char newLA[MAX_SYMBOLS] = "";
                            if (it.dotPos + 1 < strlen(rhs))
                            {
                                char beta = rhs[it.dotPos + 1];
                                if (isNonTerminal(beta))
                                {
                                    int idxB = idxNonTerm(beta);
                                    for (int k = 0; k < strlen(firstSets[idxB]); k++)
                                        addSymbol(newLA, firstSets[idxB][k]);
                                }
                                else
                                    addSymbol(newLA, beta);
                            }
                            else
                                strcpy(newLA, it.lookahead);
                            int exists = 0;
                            for (int q = 0; q < J.count; q++)
                            {
                                if (J.items[q].prodIndex == p && J.items[q].dotPos == 0)
                                {
                                    for (int k = 0; k < strlen(newLA); k++)
                                        addSymbol(J.items[q].lookahead, newLA[k]);
                                    exists = 1;
                                }
                            }
                            if (!exists)
                            {
                                J.items[J.count].prodIndex = p;
                                J.items[J.count].dotPos = 0;
                                strcpy(J.items[J.count].lookahead, newLA);
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
ItemSetLR1 goTo(ItemSetLR1 I, char X)
{
    ItemSetLR1 J;
    J.count = 0;
    for (int i = 0; i < I.count; i++)
    {
        ItemLR1 it = I.items[i];
        char *rhs = prods[it.prodIndex].rhs;
        if (it.dotPos < strlen(rhs) && rhs[it.dotPos] == X)
        {
            J.items[J.count].prodIndex = it.prodIndex;
            J.items[J.count].dotPos = it.dotPos + 1;
            strcpy(J.items[J.count].lookahead, it.lookahead);
            J.count++;
        }
    }
    return closure(J);
}
int equalSets(ItemSetLR1 A, ItemSetLR1 B)
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
                int k, match = 1;
                for (k = 0; k < strlen(A.items[i].lookahead); k++)
                    if (!contains(B.items[j].lookahead, A.items[i].lookahead[k]))
                        match = 0;
                if (match)
                {
                    found = 1;
                    break;
                }
            }
        }
        if (!found)
            return 0;
    }
    return 1;
}
void printItemSet(ItemSetLR1 I, int stateNum)
{
    printf("\nState I%d:\n", stateNum);
    for (int i = 0; i < I.count; i++)
    {
        ItemLR1 it = I.items[i];
        char *rhs = prods[it.prodIndex].rhs;
        printf("%c -> ", prods[it.prodIndex].lhs);
        for (int j = 0; j < strlen(rhs); j++)
        {
            if (j == it.dotPos)
                printf(".");
            printf("%c", rhs[j]);
        }
        if (it.dotPos == strlen(rhs))
            printf(".");
        printf(", lookahead: {");
        for (int k = 0; k < strlen(it.lookahead); k++)
            printf("%c", it.lookahead[k]);
        printf("}\n");
    }
}
void buildStates()
{
    ItemSetLR1 I0;
    I0.count = 1;
    I0.items[0].prodIndex = 0;
    I0.items[0].dotPos = 0;
    I0.items[0].lookahead[0] = '$';
    I0.items[0].lookahead[1] = '\0';
    states[0] = closure(I0);
    nStates = 1;
    printItemSet(states[0], 0);
    for (int i = 0; i < nStates; i++)
    {
        for (int s = 0; s < nNonTerms + nTerms; s++)
        {
            char X = (s < nNonTerms) ? nonTerminals[s] : terminals[s - nNonTerms];
            ItemSetLR1 J = goTo(states[i], X);
            if (J.count == 0)
                continue;
            int exists = -1;
            for (int k = 0; k < nStates; k++)
                if (equalSets(states[k], J))
                {
                    exists = k;
                    break;
                }
            if (exists == -1)
            {
                states[nStates] = J;
                transitions[i][s] = nStates;
                printItemSet(J, nStates);
                nStates++;
            }
            else
                transitions[i][s] = exists;
        }
    }
}
void printCLRTable()
{
    printf("\nCLR Parsing Table:\n");
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
        for (int t = 0; t < nTerms; t++)
        {
            char a = terminals[t];
            char entry[10] = "-";
            if (transitions[i][symbolIndex(a)] != -1)
            {
                sprintf(entry, "s%d", transitions[i][symbolIndex(a)]);
            }
            else
            {
                for (int j = 0; j < states[i].count; j++)
                {
                    ItemLR1 it = states[i].items[j];
                    char *rhs = prods[it.prodIndex].rhs;
                    if (it.dotPos == strlen(rhs) && contains(it.lookahead, a))
                    {
                        if (it.prodIndex == 0 && a == '$')
                            strcpy(entry, "A");
                        else
                            sprintf(entry, "r%d", it.prodIndex);
                    }
                }
            }
            printf("%s\t", entry);
        }
        char entry[10] = "-";
        for (int j = 0; j < states[i].count; j++)
        {
            ItemLR1 it = states[i].items[j];
            char *rhs = prods[it.prodIndex].rhs;
            if (it.dotPos == strlen(rhs) && contains(it.lookahead, '$'))
            {
                if (it.prodIndex == 0)
                    strcpy(entry, "A");
                else
                    sprintf(entry, "r%d", it.prodIndex);
            }
        }
        printf("%s\t", entry);
        for (int nt = 0; nt < nNonTerms; nt++)
        {
            int idx = transitions[i][nt];
            if (idx != -1)
                printf("%d\t", idx);
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
    printf("Enter productions (e.g., A->CC):\n");
    for (int i = 0; i < nProds; i++)
    {
        char buf[50];
        scanf("%s", buf);
        prods[i].lhs = buf[0];
        strncpy(prods[i].rhs, buf + 3, MAX_LEN - 1);
        prods[i].rhs[MAX_LEN - 1] = '\0';
        if (!contains(nonTerminals, prods[i].lhs))
            nonTerminals[nNonTerms++] = prods[i].lhs;
        for (int j = 0; j < strlen(prods[i].rhs); j++)
            if (!isNonTerminal(prods[i].rhs[j]) && !contains(terminals, prods[i].rhs[j]))
                terminals[nTerms++] = prods[i].rhs[j];
    }
    for (int i = nProds; i > 0; i--)
        prods[i] = prods[i - 1];
    prods[0].lhs = 'S';
    sprintf(prods[0].rhs, "%c", prods[1].lhs);
    nProds++;
    for (int i = 0; i < MAX_STATES; i++)
        for (int j = 0; j < MAX_SYMBOLS; j++)
            transitions[i][j] = -1;
    computeFirstSets();
    buildStates();
    printCLRTable();
    return 0;
}