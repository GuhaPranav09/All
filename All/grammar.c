#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
bool isFinal(char final[], int f, char c)
{
    for (int i = 0; i < f; i++)
    {
        if (final[i] == c)
            return true;
    }
    return false;
}
int getIndex(char arr[], int len, char ch)
{
    for (int i = 0; i < len; i++)
    {
        if (arr[i] == ch)
            return i;
    }
    return -1;
}
int main()
{
    int m, n, f;
    printf("Enter number of states: ");
    scanf("%d", &m);
    char states[m];
    for (int i = 0; i < m; i++)
    {
        printf("Enter state %d: ", i + 1);
        scanf(" %c", &states[i]);
    }
    printf("Enter number of alphabets: ");
    scanf("%d", &n);
    char alphabets[n];
    for (int i = 0; i < n; i++)
    {
        printf("Enter alphabet %d: ", i + 1);
        scanf(" %c", &alphabets[i]);
    }
    printf("Enter number of final states: ");
    scanf("%d", &f);
    char final[f];
    for (int i = 0; i < f; i++)
    {
        printf("Enter final state %d: ", i + 1);
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
    printf("\nGrammar G = (N, T, P, S)\n");
    printf("N = { ");
    for (int i = 0; i < m; i++)
        printf("%c ", states[i]);
    printf("}\n");
    printf("T = { ");
    for (int i = 0; i < n; i++)
        printf("%c ", alphabets[i]);
    printf("}\n");
    printf("S = %c\n", states[0]);
    printf("P = {\n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            char from = states[i];
            char symbol = alphabets[j];
            char to = transition[i][j];
            printf(" %c → %c%c\n", from, symbol, to);
            if (isFinal(final, f, to))
            {
                printf(" %c → %c\n", from, symbol);
            }
        }
        if (isFinal(final, f, states[i]))
        {
            printf(" %c → ε\n", states[i]);
        }
    }
    printf("}\n");
    return 0;
}