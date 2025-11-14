%{
#include <stdio.h>
#include <string.h>
%}

%%

^(.*bca.*)\n      { printf("Accepted, contains 'bca' \n"); }

^[abc]+\n         { printf("Rejected, doesn't contain 'bca'\n"); }

.|\n              { /* Ignore other characters */ }

%%

int main() {
    printf("Enter strings over {a, b, c} (one per line):\n");
    yylex();
    return 0;
}
