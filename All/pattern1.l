%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
int count_a = 0;
int count_b = 0;
%}

%%

a       { count_a++; return A; }
b       { count_b++; return B; }
[ \t]+  ;
\n      { return '\n'; }
.       { printf("Invalid character\n"); exit(1); }

%%

int yywrap() { return 1; }
