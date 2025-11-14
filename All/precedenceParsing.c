#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SIZE 50
#define N 6
char symbols[] = {'+', '*', 'i', '(', ')', '$'};
char table[N][N];
int getIndex(char c)
{
    for (int i = 0; i < N; i++)
    {
        if (symbols[i] == c)
            return i;
    }
    return -1;
}
int main()
{
    char input[SIZE], stack[SIZE];
    int top = 0, i = 0;
    int accepted = 0;
    printf("Enter the operator precedence table (6x6) for symbols [+ * i ( ) $]:\n");
    printf("Enter one row at a time (use <, >, =, e, a):\n\n");
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            scanf(" %c", &table[r][c]);
        }
    }
    printf("\nEntered Operator Precedence Table:\n ");
    for (int c = 0; c < N; c++)
        printf("%c ", symbols[c]);
    printf("\n");
    for (int r = 0; r < N; r++)
    {
        printf("%c: ", symbols[r]);
        for (int c = 0; c < N; c++)
        {
            printf("%c ", table[r][c]);
        }
        printf("\n");
    }
    printf("\nEnter the input expression ending with $: ");
    scanf("%s", input);
    stack[top] = '$';
    stack[top + 1] = '\0';
    printf("\n%-20s %-20s %-10s\n", "Stack", "Input", "Action");
    printf("-------------------------------------------------------\n");
    while (1)
    {
        int sIndex = getIndex(stack[top]);
        int iIndex = getIndex(input[i]);
        if (sIndex == -1 || iIndex == -1)
        {
            printf("Invalid symbol encountered!\n");
            accepted = 0;
            break;
        }
        char relation = table[sIndex][iIndex];
        printf("%-20s %-20s ", stack, input + i);
        if (relation == '<' || relation == '=')
        {
            stack[++top] = input[i++];
            stack[top + 1] = '\0';
            printf("Shift\n");
        }
        else if (relation == '>')
        {
            printf("Reduce\n");
            stack[top] = '\0';
            top--;
        }
        else if (relation == 'a')
        {
            printf("Accept\n");
            accepted = 1;
            break;
        }
        else
        {
            printf("Error\n");
            accepted = 0;
            break;
        }
    }
    if (accepted)
        printf("\nResult: The string is ACCEPTED\n");
    else
        printf("\nResult: The string is REJECTED\n");
    return 0;
}