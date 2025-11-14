%{
#include "y.tab.h"
#include <string.h>
#include <stdio.h>

int yylex(void);
%}

%%
[a-zA-Z][a-zA-Z0-9]*   { yylval.s = strdup(yytext); return ID; }
[0-9]+                  { yylval.s = strdup(yytext); return NUM; }
"="                     { return ASSIGN; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MUL; }
"/"                     { return DIV; }
";"                     { return SEMI; }
\n                      ;   /* Ignore newlines */
[ \t\r]                 ;   /* Ignore whitespace */
.                       ;
%%

int yywrap() { return 1; }
