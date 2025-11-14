#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 1000
#define MAX_LEN 100

char tokens[MAX_TOKENS][MAX_LEN];
int token_count = 0;
int i;

void store_token(const char *tok) {
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count++], tok);
    }
}

int isKeyword(const char *word) {
    const char *keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default",
        "do", "double", "else", "enum", "extern", "float", "for",
        "goto", "if", "int", "long", "register", "return", "short",
        "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while", "include"
    };
    int n = sizeof(keywords) / sizeof(keywords[0]);
    for ( i = 0; i < n; i++) {
        if (strcmp(word, keywords[i]) == 0) return 1;
    }
    return 0;
}

const char* classifyOperator(const char *op) {
    const char *arith[] = {"+", "-", "*", "/", "%"};
    for (int i = 0; i < 5; i++) if (strcmp(op, arith[i]) == 0) return "Arithmetic Operator";

    const char *rel[] = {"==", "!=", ">", "<", ">=", "<="};
    for (int i = 0; i < 6; i++) if (strcmp(op, rel[i]) == 0) return "Relational Operator";

    const char *logic[] = {"&&", "||", "!"};
    for (int i = 0; i < 3; i++) if (strcmp(op, logic[i]) == 0) return "Logical Operator";

    const char *assign[] = {"=", "+=", "-=", "*=", "/=", "%="};
    for (int i = 0; i < 6; i++) if (strcmp(op, assign[i]) == 0) return "Assignment Operator";

    return "Symbol";
}

int main() {
    FILE *fp = fopen("input.c", "r");
    if (!fp) {
        printf("Error: Could not open input.c\n");
        return 1;
    }

    char input[5000] = "";
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        strcat(input, line);
    }
    fclose(fp);

    printf("Program from file:\n%s\n", input);

    char temp[MAX_LEN] = "";
    int len = strlen(input);
    int i = 0;

    while (i <= len) {
        if (isalnum(input[i]) || input[i] == '_') {
            int l = strlen(temp);
            temp[l] = input[i];
            temp[l+1] = '\0';
            i++;
        }
        else {
            if (strlen(temp) > 0) {
                store_token(temp);
                temp[0] = '\0';
            }

            if (!isspace(input[i]) && input[i] != '\0') {
                if (input[i] == '"') {
                    char str[MAX_LEN] = {0};
                    int j = 0;
                    str[j++] = input[i++]; 
                    while (i < len && input[i] != '"') {
                        str[j++] = input[i++];
                    }
                    if (i < len) str[j++] = input[i++];
                    str[j] = '\0';
                    store_token(str);
                }
                else if (input[i] == '\'') {
                    char chlit[MAX_LEN] = {0};
                    int j = 0;
                    chlit[j++] = input[i++];
                    while (i < len && input[i] != '\'') {
                        chlit[j++] = input[i++];
                    }
                    if (i < len) chlit[j++] = input[i++];
                    chlit[j] = '\0';
                    store_token(chlit);
                }
                else {
                    char op[3] = {0};
                    op[0] = input[i];
                    op[1] = '\0';

                    if ((input[i] == '=' && input[i+1] == '=') ||
                        (input[i] == '!' && input[i+1] == '=') ||
                        (input[i] == '>' && input[i+1] == '=') ||
                        (input[i] == '<' && input[i+1] == '=') ||
                        (input[i] == '&' && input[i+1] == '&') ||
                        (input[i] == '|' && input[i+1] == '|') ||
                        (input[i] == '+' && input[i+1] == '=') ||
                        (input[i] == '-' && input[i+1] == '=') ||
                        (input[i] == '*' && input[i+1] == '=') ||
                        (input[i] == '/' && input[i+1] == '=') ||
                        (input[i] == '%' && input[i+1] == '=')) 
                    {
                        op[0] = input[i];
                        op[1] = input[i+1];
                        op[2] = '\0';
                        i++;
                    }
                    store_token(op);
                    i++;
                }
            }
            else {
                i++;
            }
        }
    }

    printf("\nTokens:\n");
    for (i = 0; i < token_count; i++) {
        if (isKeyword(tokens[i])) {
            printf("%s -> Keyword\n", tokens[i]);
        }
        else if (isdigit(tokens[i][0]) || 
                 (tokens[i][0] == '"' && tokens[i][strlen(tokens[i])-1] == '"') || 
                 (tokens[i][0] == '\'' && tokens[i][strlen(tokens[i])-1] == '\'')) {
            printf("%s -> Literal\n", tokens[i]);
        }
        else if (isalpha(tokens[i][0]) || tokens[i][0] == '_') {
            printf("%s -> Identifier\n", tokens[i]);
        }
        else {
            printf("%s -> %s\n", tokens[i], classifyOperator(tokens[i]));
        }
    }

    return 0;
}
