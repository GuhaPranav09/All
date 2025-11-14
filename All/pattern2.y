%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%token A B
%start lines

%%

lines:
      /* empty */
    | lines line
    ;

line: A B repeat1 B B A repeat2 '\n' { printf("Accepted\n"); }
    | A B B B A '\n'                 { printf("Accepted\n"); }
    ;

repeat1:
      B B A A repeat1
    | /* empty */
    ;

repeat2:
      B A repeat2
    | /* empty */
    ;

%%

int main() {
    printf("Enter strings (Ctrl+D to end):\n");
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    printf("Rejected: syntax error\n");
}
