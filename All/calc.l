%{
#include "y.tab.h"
#include <stdlib.h>
%}

%%

[0-9]+      { yylval = atoi(yytext); return NUMBER; }
[ \t]+      ;                     /* ignore whitespace */
\n          { return '\n'; }

"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return MUL; }
"/"         { return DIV; }
"("         { return LPAREN; }
")"         { return RPAREN; }

.           { return yytext[0]; } /* catch-all for other chars */

%%

int yywrap() {
    return 1;
}
