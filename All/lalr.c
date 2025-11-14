#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_PRODS 20
#define MAX_LEN 30
#define MAX_STATES 100
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
typedef struct
{
    int prodIndex;
    int dotPos;
} ItemLR0;
typedef struct
{
    ItemLR0 items[MAX_ITEMS];
    int count;
} CoreSet;
typedef struct
{
    int lalrState;
    int originalStates[MAX_STATES];
    int numOriginal;
} MergeInfo;
Production prods[MAX_PRODS];
int nProds;
char terminals[MAX_SYMBOLS];
int nTerms = 0;
char nonTerminals[MAX_SYMBOLS];
int nNonTerms = 0;
ItemSetLR1 lr1States[MAX_STATES];
int nLR1States = 0;
int lr1Transitions[MAX_STATES][MAX_SYMBOLS];
ItemSetLR1 lalrStates[MAX_STATES];
int nLALRStates = 0;
int lalrTransitions[MAX_STATES][MAX_SYMBOLS];
MergeInfo mergeTable[MAX_STATES];
int nMergeEntries = 0;
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
            {
                addSymbol(firstSets[idxA], rhs[0]);
            }
            if (strcmp(old, firstSets[idxA]) != 0)
                changed = 1;
        }
    }
}
CoreSet extractCore(ItemSetLR1 I)
{
    CoreSet core;
    core.count = 0;
    for (int i = 0; i < I.count; i++)
    {
        int exists = 0;
        for (int j = 0; j < core.count; j++)
        {
            if (core.items[j].prodIndex == I.items[i].prodIndex &&
                core.items[j].dotPos == I.items[i].dotPos)
            {
                exists = 1;
                break;
            }
        }
        if (!exists)
        {
            core.items[core.count].prodIndex = I.items[i].prodIndex;
            core.items[core.count].dotPos = I.items[i].dotPos;
            core.count++;
        }
    }
    return core;
}
int sameCores(CoreSet A, CoreSet B)
{
    if (A.count != B.count)
        return 0;
    for (int i = 0; i < A.count; i++)
    {
        int found = 0;
        for (int j = 0; j < B.count; j++)
        {
            if (A.items[i].prodIndex == B.items[j].prodIndex &&
                A.items[i].dotPos == B.items[j].dotPos)
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
int equalItemSets(ItemSetLR1 A, ItemSetLR1 B)
{
    if (A.count != B.count)
        return 0;
    for (int i = 0; i < A.count; i++)
    {
        int found = 0;
        for (int j = 0; j < B.count; j++)
        {
            if (A.items[i].prodIndex == B.items[j].prodIndex &&
                A.items[i].dotPos == B.items[j].dotPos &&
                strcmp(A.items[i].lookahead, B.items[j].lookahead) == 0)
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
ItemSetLR1 mergeLookaheads(ItemSetLR1 A, ItemSetLR1 B)
{
    ItemSetLR1 merged = A;
    for (int i = 0; i < B.count; i++)
    {
        int found = 0;
        for (int j = 0; j < merged.count; j++)
        {
            if (merged.items[j].prodIndex == B.items[i].prodIndex &&
                merged.items[j].dotPos == B.items[i].dotPos)
            {
                for (int k = 0; k < strlen(B.items[i].lookahead); k++)
                {
                    addSymbol(merged.items[j].lookahead, B.items[i].lookahead[k]);
                }
                found = 1;
                break;
            }
        }
        if (!found)
        {
            merged.items[merged.count] = B.items[i];
            merged.count++;
        }
    }
    return merged;
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
                                {
                                    addSymbol(newLA, beta);
                                }
                            }
                            else
                            {
                                strcpy(newLA, it.lookahead);
                            }
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
void buildLR1States()
{
    for (int i = 0; i < MAX_STATES; i++)
    {
        for (int j = 0; j < MAX_SYMBOLS; j++)
        {
            lr1Transitions[i][j] = -1;
        }
    }
    ItemSetLR1 I0;
    I0.count = 1;
    I0.items[0].prodIndex = 0;
    I0.items[0].dotPos = 0;
    I0.items[0].lookahead[0] = '$';
    I0.items[0].lookahead[1] = '\0';
    lr1States[0] = closure(I0);
    nLR1States = 1;
    for (int i = 0; i < nLR1States; i++)
    {
        for (int s = 0; s < nNonTerms + nTerms; s++)
        {
            char X = (s < nNonTerms) ? nonTerminals[s] : terminals[s - nNonTerms];
            ItemSetLR1 J = goTo(lr1States[i], X);
            if (J.count == 0)
                continue;
            int exists = -1;
            for (int k = 0; k < nLR1States; k++)
            {
                if (equalItemSets(lr1States[k], J))
                {
                    exists = k;
                    break;
                }
            }
            if (exists == -1)
            {
                lr1States[nLR1States] = J;
                lr1Transitions[i][s] = nLR1States;
                nLR1States++;
            }
            else
            {
                lr1Transitions[i][s] = exists;
            }
        }
    }
}
void buildLALRStates()
{
    for (int i = 0; i < MAX_STATES; i++)
    {
        for (int j = 0; j < MAX_SYMBOLS; j++)
        {
            lalrTransitions[i][j] = -1;
        }
    }
    nLALRStates = 0;
    nMergeEntries = 0;
    for (int i = 0; i < nLR1States; i++)
    {
        CoreSet currentCore = extractCore(lr1States[i]);
        int merged = -1;
        for (int j = 0; j < nLALRStates; j++)
        {
            CoreSet existingCore = extractCore(lalrStates[j]);
            if (sameCores(currentCore, existingCore))
            {
                merged = j;
                break;
            }
        }
        if (merged == -1)
        {
            lalrStates[nLALRStates] = lr1States[i];
            mergeTable[nMergeEntries].lalrState = nLALRStates;
            mergeTable[nMergeEntries].originalStates[0] = i;
            mergeTable[nMergeEntries].numOriginal = 1;
            nMergeEntries++;
            nLALRStates++;
        }
        else
        {
            lalrStates[merged] = mergeLookaheads(lalrStates[merged], lr1States[i]);
            for (int k = 0; k < nMergeEntries; k++)
            {
                if (mergeTable[k].lalrState == merged)
                {
                    mergeTable[k].originalStates[mergeTable[k].numOriginal] = i;
                    mergeTable[k].numOriginal++;
                    break;
                }
            }
        }
    }
    for (int i = 0; i < nLALRStates; i++)
    {
        for (int s = 0; s < nNonTerms + nTerms; s++)
        {
            char X = (s < nNonTerms) ? nonTerminals[s] : terminals[s - nNonTerms];
            ItemSetLR1 J = goTo(lalrStates[i], X);
            if (J.count == 0)
                continue;
            CoreSet gotoCore = extractCore(J);
            for (int k = 0; k < nLALRStates; k++)
            {
                CoreSet targetCore = extractCore(lalrStates[k]);
                if (sameCores(gotoCore, targetCore))
                {
                    lalrTransitions[i][s] = k;
                    break;
                }
            }
        }
    }
}
void printLALRStates()
{
    printf("\n=== LALR STATES (After Merging) ===\n");
    for (int i = 0; i < nLALRStates; i++)
    {
        printf("\nLALR State I%d:\n", i);
        for (int j = 0; j < lalrStates[i].count; j++)
        {
            ItemLR1 it = lalrStates[i].items[j];
            char *rhs = prods[it.prodIndex].rhs;
            printf("%c -> ", prods[it.prodIndex].lhs);
            for (int k = 0; k < strlen(rhs); k++)
            {
                if (k == it.dotPos)
                    printf(".");
                printf("%c", rhs[k]);
            }
            if (it.dotPos == strlen(rhs))
                printf(".");
            printf(", lookahead: {");
            for (int k = 0; k < strlen(it.lookahead); k++)
            {
                printf("%c", it.lookahead[k]);
                if (k < strlen(it.lookahead) - 1)
                    printf(",");
            }
            printf("}\n");
        }
    }
}
void printMergeTable()
{
    printf("\n=== LALR STATE MERGING INFORMATION ===\n");
    printf("LALR State\tMerged from LR(1) States\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < nMergeEntries; i++)
    {
        printf("I%d\t\t", mergeTable[i].lalrState);
        for (int j = 0; j < mergeTable[i].numOriginal; j++)
        {
            printf("I%d", mergeTable[i].originalStates[j]);
            if (j < mergeTable[i].numOriginal - 1)
                printf(", ");
        }
        printf("\n");
    }
    printf("\nTotal LR(1) states: %d\n", nLR1States);
    printf("Total LALR states: %d\n", nLALRStates);
    printf("States saved: %d\n", nLR1States - nLALRStates);
}
void printLALRTable()
{
    printf("\n=== LALR PARSING TABLE ===\n");
    printf("STATE\t");
    for (int i = 0; i < nTerms; i++)
        printf("%c\t", terminals[i]);
    printf("$\t");
    for (int i = 0; i < nNonTerms; i++)
        printf("%c\t", nonTerminals[i]);
    printf("\n");
    for (int i = 0; i < nLALRStates; i++)
    {
        printf("%d\t", i);
        for (int t = 0; t < nTerms; t++)
        {
            char a = terminals[t];
            char entry[10] = "-";
            if (lalrTransitions[i][symbolIndex(a)] != -1)
            {
                sprintf(entry, "s%d", lalrTransitions[i][symbolIndex(a)]);
            }
            else
            {
                for (int j = 0; j < lalrStates[i].count; j++)
                {
                    ItemLR1 it = lalrStates[i].items[j];
                    char *rhs = prods[it.prodIndex].rhs;
                    if (it.dotPos == strlen(rhs) && contains(it.lookahead, a))
                    {
                        if (it.prodIndex == 0 && a == '$')
                        {
                            strcpy(entry, "A");
                        }
                        else
                        {
                            sprintf(entry, "r%d", it.prodIndex);
                        }
                    }
                }
            }
            printf("%s\t", entry);
        }
        char entry[10] = "-";
        for (int j = 0; j < lalrStates[i].count; j++)
        {
            ItemLR1 it = lalrStates[i].items[j];
            char *rhs = prods[it.prodIndex].rhs;
            if (it.dotPos == strlen(rhs) && contains(it.lookahead, '$'))
            {
                if (it.prodIndex == 0)
                {
                    strcpy(entry, "A");
                }
                else
                {
                    sprintf(entry, "r%d", it.prodIndex);
                }
            }
        }
        printf("%s\t", entry);
        for (int nt = 0; nt < nNonTerms; nt++)
        {
            int idx = lalrTransitions[i][nt];
            if (idx != -1)
            {
                printf("%d\t", idx);
            }
            else
            {
                printf("-\t");
            }
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
        {
            nonTerminals[nNonTerms++] = prods[i].lhs;
        }
        for (int j = 0; j < strlen(prods[i].rhs); j++)
        {
            if (!isNonTerminal(prods[i].rhs[j]) &&
                !contains(terminals, prods[i].rhs[j]))
            {
                terminals[nTerms++] = prods[i].rhs[j];
            }
        }
    }
    for (int i = nProds; i > 0; i--)
    {
        prods[i] = prods[i - 1];
    }
    prods[0].lhs = 'S';
    sprintf(prods[0].rhs, "%c", prods[1].lhs);
    nProds++;
    computeFirstSets();
    buildLR1States();
    buildLALRStates();
    printLALRStates();
    printMergeTable();
    printLALRTable();
    return 0;
}
