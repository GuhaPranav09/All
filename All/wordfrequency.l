%{
#include <stdio.h>
#include <string.h>

char target[100];
int count = 0;
%}

%%
[a-zA-Z]+ {
    if (strcmp(yytext, target) == 0) {
        count++;
    }
}
.   ;   
%%

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <word> <filename>\n", argv[0]);
        return 1;
    }

    strcpy(target, argv[1]);
    yyin = fopen(argv[2], "r");
    if (!yyin) {
        printf("Error: could not open file %s\n", argv[2]);
        return 1;
    }

    yylex();
    printf("The word '%s' occurred %d times in file %s\n", target, count, argv[2]);

    fclose(yyin);
    return 0;
}
