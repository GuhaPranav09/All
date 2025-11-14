#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
int getIndex(char alphabets[], int n, char c)
{
    for (int i = 0; i < n; i++)
    {
        if (alphabets[i] == c)
            return i;
    }
    return -1;
}
int getStateIndex(char states[], int m, char c)
{
    for (int i = 0; i < m; i++)
    {
        if (states[i] == c)
            return i;
    }
    return -1;
}
bool isFinal(char final[], int f, char c)
{
    for (int i = 0; i < f; i++)
    {
        if (final[i] == c)
            return true;
    }
    return false;
}
int main()
{
    int m, n, f;
    printf("Enter number of states: ");
    scanf("%d", &m);
    char states[m];
    for (int i = 0; i < m; i++)
    {
        printf("Enter state %d: ", (i + 1));
        scanf(" %c", &states[i]);
    }
    printf("Enter number of alphabets: ");
    scanf("%d", &n);
    char alphabets[n];
    for (int i = 0; i < n; i++)
    {
        printf("Enter alphabet %d: ", (i + 1));
        scanf(" %c", &alphabets[i]);
    }
    printf("Enter number of final states: ");
    scanf("%d", &f);
    char final[f];
    for (int i = 0; i < f; i++)
    {
        printf("Enter final state %d: ", (i + 1));
        scanf(" %c", &final[i]);
    }
    char transition[m][n];
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("Enter transition from %c on %c: ", states[i], alphabets[j]);
            scanf(" %c", &transition[i][j]);
        }
    }
    printf("\nTransition Table:\n");
    printf("State\t");
    for (int j = 0; j < n; j++)
    {
        printf("%c\t", alphabets[j]);
    }
    printf("\n");
    for (int i = 0; i < m; i++)
    {
        printf("%c\t", states[i]);
        for (int j = 0; j < n; j++)
        {
            printf("%c\t", transition[i][j]);
        }
        printf("\n");
    }
    char inp[30];
    printf("\nEnter string: ");
    scanf("%s", inp);
    int currentState = 0;
    printf("\nTransitions:\n");
    printf("Start at state %c\n", states[currentState]);
    for (int i = 0; i < strlen(inp); i++)
    {
        int index = getIndex(alphabets, n, inp[i]);
        if (index == -1)
        {
            printf("Error: Input symbol %c not in alphabet\n", inp[i]);
            return 1;
        }
        int nextState = getStateIndex(states, m, transition[currentState][index]);
        if (nextState == -1)
        {
            printf("No transition from state %c on input %c. String rejected.\n", states[currentState], inp[i]);
            return 0;
        }
        printf("On input %c, move from state %c to state %c\n", inp[i], states[currentState], states[nextState]);
        currentState = nextState;
    }
    if (isFinal(final, f, states[currentState]))
    {
        printf("String accepted. Reached final state %c.\n", states[currentState]);
    }
    else
    {
        printf("String rejected. Ended at state %c.\n", states[currentState]);
    }
    return 0;
}
