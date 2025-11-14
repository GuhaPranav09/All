#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 100
#define MAX_LEN 50
#define MAX_LINE_LEN 200

char tokens[MAX_TOKENS][MAX_LEN];
int token_count = 0;

void store_token(const char *tok) {
    if (token_count < MAX_TOKENS && strlen(tok) > 0) {
        strcpy(tokens[token_count++], tok);
    }
}

int isKeyword(const char *word) {
    const char *keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default",
        "do", "double", "else", "enum", "extern", "float", "for",
        "goto", "if", "int", "long", "register", "return", "short",
        "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while"
    };
    int n = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < n; i++) {
        if (strcmp(word, keywords[i]) == 0) return 1;
    }
    return 0;
}

const char* classifyOperator(const char *op) {
    // Arithmetic
    const char *arith[] = {"+", "-", "*", "/", "%", "++", "--"};
    for (int i = 0; i < 7; i++) if (strcmp(op, arith[i]) == 0) return "Arithmetic Operator";

    // Relational
    const char *rel[] = {"==", "!=", ">", "<", ">=", "<="};
    for (int i = 0; i < 6; i++) if (strcmp(op, rel[i]) == 0) return "Relational Operator";

    // Logical
    const char *logic[] = {"&&", "||", "!"};
    for (int i = 0; i < 3; i++) if (strcmp(op, logic[i]) == 0) return "Logical Operator";

    // Assignment
    const char *assign[] = {"=", "+=", "-=", "*=", "/=", "%="};
    for (int i = 0; i < 6; i++) if (strcmp(op, assign[i]) == 0) return "Assignment Operator";

    return "Symbol";
}

void tokenize_line(const char *input) {
    char temp[MAX_LEN] = "";
    int len = strlen(input);

    for (int i = 0; i < len; i++) {
        if (isalnum((unsigned char)input[i]) || input[i] == '_') {
            int l = strlen(temp);
            temp[l] = input[i];
            temp[l+1] = '\0';
        } else {
            if (strlen(temp) > 0) {
                store_token(temp);
                temp[0] = '\0';
            }

            if (!isspace((unsigned char)input[i])) {
                // String literal
                if (input[i] == '"') {
                    char str[MAX_LEN] = {0};
                    int j = 0;
                    str[j++] = input[i++];
                    while (i < len && input[i] != '"') {
                        str[j++] = input[i++];
                    }
                    if (i < len) str[j++] = input[i];
                    str[j] = '\0';
                    store_token(str);
                }
                // Char literal
                else if (input[i] == '\'') {
                    char chlit[MAX_LEN] = {0};
                    int j = 0;
                    chlit[j++] = input[i++];
                    while (i < len && input[i] != '\'') {
                        chlit[j++] = input[i++];
                    }
                    if (i < len) chlit[j++] = input[i];
                    chlit[j] = '\0';
                    store_token(chlit);
                }
                else {
                    // Handle multi-character operators
                    char op[3] = {input[i], '\0', '\0'};
                    if ((input[i] == '=' && input[i+1] == '=') ||
                        (input[i] == '!' && input[i+1] == '=') ||
                        (input[i] == '>' && input[i+1] == '=') ||
                        (input[i] == '<' && input[i+1] == '=') ||
                        (input[i] == '&' && input[i+1] == '&') ||
                        (input[i] == '|' && input[i+1] == '|') ||
                        (input[i] == '+' && (input[i+1] == '+' || input[i+1] == '=')) ||
                        (input[i] == '-' && (input[i+1] == '-' || input[i+1] == '=')) ||
                        (input[i] == '*' && input[i+1] == '=') ||
                        (input[i] == '/' && input[i+1] == '=') ||
                        (input[i] == '%' && input[i+1] == '=')) 
                    {
                        op[1] = input[i+1];
                        i++;
                    }
                    store_token(op);
                }
            }
        }
    }

    if (strlen(temp) > 0) {
        store_token(temp);
    }
}

int main() {
    char line[MAX_LINE_LEN];
    FILE *fp = fopen("output.txt", "w");
    if (!fp) {
        printf("Error opening output.txt for writing.\n");
        return 1;
    }

    printf("Enter code (type END on a new line to stop):\n");

    while (1) {
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\r\n")] = '\0'; // remove CR+LF

        if (strcmp(line, "END") == 0) break;

        tokenize_line(line);
    }

    printf("\nTokens:\n");
    fprintf(fp, "Tokens:\n");

    for (int i = 0; i < token_count; i++) {
        if (isKeyword(tokens[i])) {
            printf("%s -> Keyword\n", tokens[i]);
            fprintf(fp, "%s -> Keyword\n", tokens[i]);
        }
        else if (isdigit((unsigned char)tokens[i][0]) ||
                 (tokens[i][0] == '"' && tokens[i][strlen(tokens[i])-1] == '"') ||
                 (tokens[i][0] == '\'' && tokens[i][strlen(tokens[i])-1] == '\'')) {
            printf("%s -> Literal\n", tokens[i]);
            fprintf(fp, "%s -> Literal\n", tokens[i]);
        }
        else if (isalpha((unsigned char)tokens[i][0]) || tokens[i][0] == '_') {
            printf("%s -> Identifier\n", tokens[i]);
            fprintf(fp, "%s -> Identifier\n", tokens[i]);
        }
        else {
            printf("%s -> %s\n", tokens[i], classifyOperator(tokens[i]));
            fprintf(fp, "%s -> %s\n", tokens[i], classifyOperator(tokens[i]));
        }
    }

    fclose(fp);
    printf("\nOutput also saved to output.txt\n");

    return 0;
}
