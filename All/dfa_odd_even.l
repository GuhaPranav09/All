%{
#include <stdio.h>
int a_count = 0;
int b_count = 0;
%}

%%
a    { a_count++; }
b    { b_count++; }
\n   {
        if (a_count % 2 == 1 && b_count % 2 == 0)
            printf("Accepted: Odd number of a's and Even number of b's\n");
        else
            printf("Rejected\n");

        // Reset for next input line
        a_count = 0;
        b_count = 0;
    }
.    ;  // Ignore any other character
%%

int main() {
    printf("Enter strings (Ctrl+D to end):\n");
    yylex();
    return 0;
}
