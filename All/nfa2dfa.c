#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX 100
int nfaStates, alphabets;
char states[MAX], alphabet[MAX];
char nfa[MAX][MAX][MAX];
int dfaTrans[MAX][MAX];
char dfaStates[MAX][MAX];
bool visited[MAX];
int dfaCount = 0;
char dfaStateNames[MAX];
bool isPresent(char dfaStates[][MAX], int count, char *state)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(dfaStates[i], state) == 0)
            return true;
    }
    return false;
}
int getStateIndex(char dfaStates[][MAX], int count, char *state)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(dfaStates[i], state) == 0)
            return i;
    }
    return -1;
}
void sortString(char *str)
{
    for (int i = 0; i < strlen(str) - 1; i++)
    {
        for (int j = i + 1; j < strlen(str); j++)
        {
            if (str[i] > str[j])
            {
                char tmp = str[i];
                str[i] = str[j];
                str[j] = tmp;
            }
        }
    }
}
void constructDFA()
{
    dfaCount = 1;
    dfaStates[0][0] = states[0];
    dfaStates[0][1] = '\0';
    int i = 0;
    while (i < dfaCount)
    {
        visited[i] = true;
        for (int a = 0; a < alphabets; a++)
        {
            char newState[MAX] = "";
            for (int s = 0; s < strlen(dfaStates[i]); s++)
            {
                char from = dfaStates[i][s];
                char *ptr = strchr(states, from);
                if (!ptr)
                    continue;
                int fromIndex = ptr - states;
                strcat(newState, nfa[fromIndex][a]);
            }
            sortString(newState);
            if (strlen(newState) == 0)
            {
                dfaTrans[i][a] = -1;
            }
            else
            {
                if (!isPresent(dfaStates, dfaCount, newState))
                {
                    strcpy(dfaStates[dfaCount++], newState);
                }
                dfaTrans[i][a] = getStateIndex(dfaStates, dfaCount, newState);
            }
        }
        i++;
    }
    bool needDeadState = false;
    for (int i = 0; i < dfaCount; i++)
    {
        for (int a = 0; a < alphabets; a++)
        {
            if (dfaTrans[i][a] == -1)
            {
                needDeadState = true;
            }
        }
    }
    if (needDeadState)
    {
        strcpy(dfaStates[dfaCount], "#");
        int deadIndex = dfaCount++;
        for (int a = 0; a < alphabets; a++)
        {
            dfaTrans[deadIndex][a] = deadIndex;
        }
        for (int i = 0; i < dfaCount; i++)
        {
            for (int a = 0; a < alphabets; a++)
            {
                if (dfaTrans[i][a] == -1)
                {
                    dfaTrans[i][a] = deadIndex;
                }
            }
        }
    }
}
void assignStateNames()
{
    char name = 'P';
    for (int i = 0; i < dfaCount; i++)
    {
        dfaStateNames[i] = name++;
    }
}
bool isFinalDFAState(char *dfaState, char *finalStates, int fCount)
{
    for (int i = 0; i < fCount; i++)
    {
        if (strchr(dfaState, finalStates[i]))
        {
            return true;
        }
    }
    return false;
}
int main()
{
    int f;
    char finalStates[MAX];
    printf("Enter number of NFA states: ");
    scanf("%d", &nfaStates);
    for (int i = 0; i < nfaStates; i++)
    {
        printf("Enter state %d: ", i + 1);
        scanf(" %c", &states[i]);
    }
    printf("Enter number of input alphabets: ");
    scanf("%d", &alphabets);
    for (int i = 0; i < alphabets; i++)
    {
        printf("Enter alphabet %d: ", i + 1);
        scanf(" %c", &alphabet[i]);
    }
    printf("Enter number of final states in NFA: ");
    scanf("%d", &f);
    for (int i = 0; i < f; i++)
    {
        printf("Enter final state %d: ", i + 1);
        scanf(" %c", &finalStates[i]);
    }
    for (int i = 0; i < nfaStates; i++)
    {
        for (int j = 0; j < alphabets; j++)
        {
            printf("Enter transitions from %c on %c (e.g., AB or -): ", states[i], alphabet[j]);
            scanf("%s", nfa[i][j]);
            if (strcmp(nfa[i][j], "-") == 0)
                strcpy(nfa[i][j], "");
        }
    }
    constructDFA();
    assignStateNames();
    printf("\nIntermediate DFA States:\n");
    for (int i = 0; i < dfaCount; i++)
    {
        printf("%c: {%s}\n", dfaStateNames[i], dfaStates[i]);
    }
    printf("\n Final DFA Table \n");
    printf("States: ");
    for (int i = 0; i < dfaCount; i++)
    {
        printf("%c ", dfaStateNames[i]);
    }
    printf("\nAlphabets: ");
    for (int i = 0; i < alphabets; i++)
    {
        printf("%c ", alphabet[i]);
    }
    printf("\nFinal States: ");
    for (int i = 0; i < dfaCount; i++)
    {
        if (isFinalDFAState(dfaStates[i], finalStates, f))
        {
            printf("%c ", dfaStateNames[i]);
        }
    }
    printf("\n\nDFA Transition Table:\n");
    printf("State\t");
    for (int i = 0; i < alphabets; i++)
    {
        printf("%c\t", alphabet[i]);
    }
    printf("\n");
    for (int i = 0; i < dfaCount; i++)
    {
        printf("%c\t", dfaStateNames[i]);
        for (int j = 0; j < alphabets; j++)
        {
            printf("%c\t", dfaStateNames[dfaTrans[i][j]]);
        }
        printf("\n");
    }
    return 0;
}