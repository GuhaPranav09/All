%{
#include <stdio.h>
#include <stdlib.h>

/* Node structure for AST */
typedef struct Node {
    char op;              
    int val;              
    struct Node *left;
    struct Node *right;
} Node;

int yylex();
void yyerror(const char *s);
Node* createNode(char op, int val, Node* left, Node* right);
void printPostfix(Node* node);

#define YYSTYPE Node*    /* Semantic values are AST node pointers */
%}

%token NUMBER
%token PLUS MINUS MUL DIV LPAREN RPAREN

%%

input:
    /* empty */
    | input line
    ;

line:
    expr '\n' {
        printPostfix($1);
        printf("\n");
    }
    ;

expr:
    expr PLUS term   { $$ = createNode('+', 0, $1, $3); }
    | expr MINUS term { $$ = createNode('-', 0, $1, $3); }
    | term           { $$ = $1; }
    ;

term:
    term MUL factor  { $$ = createNode('*', 0, $1, $3); }
    | term DIV factor { $$ = createNode('/', 0, $1, $3); }
    | factor         { $$ = $1; }
    ;

factor:
    NUMBER           { $$ = $1; }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

%%

Node* createNode(char op, int val, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->op = op;
    node->val = val;
    node->left = left;
    node->right = right;
    return node;
}

void printPostfix(Node* node) {
    if (node == NULL) return;
    printPostfix(node->left);
    printPostfix(node->right);
    if (node->op == '\0')
        printf("%d ", node->val);
    else
        printf("%c ", node->op);
}

int main() {
    printf("Enter arithmetic expressions (Ctrl+D to end):\n");
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
