%{
#include <stdio.h>
#include <string.h>

char oldWord[100];
char newWord[100];
%}

%%
[a-zA-Z]+ {
    if (strcmp(yytext, oldWord) == 0) {
        printf("%s", newWord);
    } else {
        printf("%s", yytext);
    }
}
[ \t\n]   { ECHO; }      
.         { ECHO; }     
%%

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <old_word> <new_word> <filename>\n", argv[0]);
        return 1;
    }

    strcpy(oldWord, argv[1]);
    strcpy(newWord, argv[2]);

    yyin = fopen(argv[3], "r");
    if (!yyin) {
        perror("Error opening file");
        return 1;
    }

    yylex();
    fclose(yyin);
    return 0;
}
