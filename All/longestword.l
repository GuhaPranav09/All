%{
#include <stdio.h>
#include <string.h>

int maxLength = 0;
char longestWord[256];
%}

%%
[a-zA-Z]+ {
    int len = strlen(yytext);
    if (len > maxLength) {
        maxLength = len;
        strcpy(longestWord, yytext);
    }
}
.   ;   // Ignore non-word characters
%%

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Error opening file");
        return 1;
    }

    yylex();
    fclose(yyin);

    printf("Longest word: %s\n", longestWord);
    printf("Length: %d\n", maxLength);

    return 0;
}
