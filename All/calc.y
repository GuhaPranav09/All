%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
void yyerror(const char *s);
%}

%token NUMBER
%token PLUS MINUS MUL DIV LPAREN RPAREN

%%

input:
      /* empty */
    | input line
    ;

line:
      expr '\n'   { printf("Result = %d\n", $1); }
    ;

expr:
      expr PLUS term   { $$ = $1 + $3; }
    | expr MINUS term  { $$ = $1 - $3; }
    | term             { $$ = $1; }
    ;

term:
      term MUL factor  { $$ = $1 * $3; }
    | term DIV factor  {
                          if ($3 == 0) {
                              printf("Error: Division by zero\n");
                              exit(1);
                          }
                          $$ = $1 / $3;
                       }
    | factor            { $$ = $1; }
    ;

factor:
      NUMBER            { $$ = $1; }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    printf("Enter expressions (Ctrl+D to exit):\n");
    yyparse();
    return 0;
}
