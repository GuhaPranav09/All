%{
#include <stdio.h>
#include <stdlib.h>

// Function prototype
int yywrap();
%}

/* Definitions */
digit       [0-9]
letter      [a-zA-Z]
id          {letter}({letter}|{digit})*
number      {digit}+
whitespace  [ \t\n]+

%%

"if"            { printf("Keyword: IF\n"); }
"else"          { printf("Keyword: ELSE\n"); }
"while"         { printf("Keyword: WHILE\n"); }
"for"           { printf("Keyword: FOR\n"); }

{id}            { printf("Identifier: %s\n", yytext); }
{number}        { printf("Number: %s\n", yytext); }
"+"             { printf("Operator: PLUS\n"); }
"-"             { printf("Operator: MINUS\n"); }
"*"             { printf("Operator: MULTIPLY\n"); }
"/"             { printf("Operator: DIVIDE\n"); }
"="             { printf("Operator: ASSIGN\n"); }

{whitespace}    { /* Ignore spaces, tabs, newlines */ }

.               { printf("Unknown token: %s\n", yytext); }

%%

int yywrap() {
    return 1;
}

int main() {
    yylex();  // Start lexical analysis
    return 0;
}
