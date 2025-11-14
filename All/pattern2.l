%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

%%

a       { return A; }
b       { return B; }
\n      { return '\n'; }
.       { printf("Invalid character\n"); exit(1); }

%%

int yywrap() { return 1; }
