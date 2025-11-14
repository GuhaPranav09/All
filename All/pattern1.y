%{
#include <stdio.h>
#include <stdlib.h>
extern int count_a, count_b;
int yylex();
void yyerror(const char *s);
%}

%token A B

%%

lines:
    /* empty */
    | lines line
    ;

line: seq '\n' {
        if (count_a != count_b)
            printf("Accepted\n");
        else
            printf("Rejected: m = n\n");
        count_a = count_b = 0;
     }
    ;

seq: /* empty */
   | seq A
   | seq B
   ;

%%
int main() {
    printf("Enter strings (Ctrl+D to end):\n");
    yyparse();  // parse all lines at once
    return 0;
}

void yyerror(const char *s) {
    printf("Rejected: syntax error\n");
}
