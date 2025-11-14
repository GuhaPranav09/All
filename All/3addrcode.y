%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
int yyerror(char *s);

int tempCount = 0;
char* newTemp();

struct expr {
    char *addr;
    char code[512];
};
%}

%union {
    char *s;
    struct expr *E;
}

%token <s> ID NUM
%token ASSIGN PLUS MINUS MUL DIV SEMI
%type <E> expr term factor

%%

program:
      /* empty */
    | program stmt
    ;

stmt:
    ID ASSIGN expr SEMI {
        printf("%s", $3->code);
        printf("%s = %s\n", $1, $3->addr);
        printf("\nEnter statement: ");  /* 👈 Prompt again for next statement */
    }
    ;

expr:
    expr PLUS term {
        $$ = malloc(sizeof(struct expr));
        char *t = newTemp();
        sprintf($$->code, "%s%s%s = %s + %s\n", $1->code, $3->code, t, $1->addr, $3->addr);
        $$->addr = strdup(t);
    }
    | expr MINUS term {
        $$ = malloc(sizeof(struct expr));
        char *t = newTemp();
        sprintf($$->code, "%s%s%s = %s - %s\n", $1->code, $3->code, t, $1->addr, $3->addr);
        $$->addr = strdup(t);
    }
    | term { $$ = $1; }
    ;

term:
    term MUL factor {
        $$ = malloc(sizeof(struct expr));
        char *t = newTemp();
        sprintf($$->code, "%s%s%s = %s * %s\n", $1->code, $3->code, t, $1->addr, $3->addr);
        $$->addr = strdup(t);
    }
    | term DIV factor {
        $$ = malloc(sizeof(struct expr));
        char *t = newTemp();
        sprintf($$->code, "%s%s%s = %s / %s\n", $1->code, $3->code, t, $1->addr, $3->addr);
        $$->addr = strdup(t);
    }
    | factor { $$ = $1; }
    ;

factor:
    ID {
        $$ = malloc(sizeof(struct expr));
        $$->addr = strdup($1);
        $$->code[0] = '\0';
    }
    | NUM {
        $$ = malloc(sizeof(struct expr));
        $$->addr = strdup($1);
        $$->code[0] = '\0';
    }
    ;
%%

char* newTemp() {
    static char buffer[10];
    sprintf(buffer, "t%d", tempCount++);
    return strdup(buffer);
}

int main() {
    printf("Enter statement: ");
    yyparse();
    printf("\nAll statements processed. Exiting.\n");
    return 0;
}

int yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
    return 0;
}
