%{
#include <stdlib.h>

/* AST node structure (must match parser) */
typedef struct Node {
    char op;              
    int val;              
    struct Node *left;
    struct Node *right;
} Node;

/* Tell lexer that YYSTYPE is Node* */
#define YYSTYPE Node*

/* Include tokens */
#include "y.tab.h"

/* Forward declaration of createNode from parser */
Node* createNode(char op, int val, Node* left, Node* right);
%}

%%

[0-9]+      { yylval = createNode('\0', atoi(yytext), NULL, NULL); return NUMBER; }
[ \t]+      ;          /* ignore spaces */
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return MUL; }
"/"         { return DIV; }
"("         { return LPAREN; }
")"         { return RPAREN; }
\n          { return '\n'; }
.           { return yytext[0]; }

%%

int yywrap() { return 1; }
