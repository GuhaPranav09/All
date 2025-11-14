#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LINES 100
#define LEN 100

int isNumber(char *str) {
    for(int i=0; str[i]; i++) {
        if(str[i] < '0' || str[i] > '9') return 0;
    }
    return 1;
}

int powerOf2(int n) {
    if(n <= 0) return -1;
    int exp = 0;
    while(n > 1) {
        if(n % 2 != 0) return -1;
        n /= 2;
        exp++;
    }
    return exp;
}

void optimizeTAC(char *result, char *op1, char op, char *op2) {
    // Constant Folding
    if(op != '\0' && isNumber(op1) && isNumber(op2)) {
        int val1 = atoi(op1);
        int val2 = atoi(op2);
        int res = 0;
        switch(op) {
            case '+': res = val1 + val2; break;
            case '-': res = val1 - val2; break;
            case '*': res = val1 * val2; break;
            case '/': res = val1 / val2; break;
        }
        printf("%s = %d\n", result, res);
        return;
    }

    // Strength Reduction
    if(op == '*') {
        if(isNumber(op2)) {
            int exp = powerOf2(atoi(op2));
            if(exp != -1) {
                printf("%s = %s << %d\n", result, op1, exp);
                return;
            }
        }
        if(isNumber(op1)) {
            int exp = powerOf2(atoi(op1));
            if(exp != -1) {
                printf("%s = %s << %d\n", result, op2, exp);
                return;
            }
        }
    }
    if(op == '/') {
        if(isNumber(op2)) {
            int exp = powerOf2(atoi(op2));
            if(exp != -1) {
                printf("%s = %s >> %d\n", result, op1, exp);
                return;
            }
        }
    }

    // Algebraic Transformations
    if(op == '*') {
        if(isNumber(op1) && atoi(op1) == 1) {
            printf("%s = %s\n", result, op2);
            return;
        }
        if(isNumber(op2) && atoi(op2) == 1) {
            printf("%s = %s\n", result, op1);
            return;
        }
    }
    if(op == '+') {
        if(isNumber(op1) && atoi(op1) == 0) {
            printf("%s = %s\n", result, op2);
            return;
        }
        if(isNumber(op2) && atoi(op2) == 0) {
            printf("%s = %s\n", result, op1);
            return;
        }
    }
    if(op == '-') {
        if(isNumber(op2) && atoi(op2) == 0) {
            printf("%s = %s\n", result, op1);
            return;
        }
    }

    if(op != '\0') {
        printf("%s = %s %c %s\n", result, op1, op, op2);
    } else {
        printf("%s = %s\n", result, op1);
    }
}

int main() {
    char input[MAX_LINES][LEN];
    char result[20], op1[20], op2[20], op;
    int lineCount = 0;

    printf("Enter TAC statements (type 'exit' to stop):\n");
    while(1) {
        fgets(input[lineCount], LEN, stdin);
        input[lineCount][strcspn(input[lineCount], "\n")] = 0;
        if(strcmp(input[lineCount], "exit") == 0) break;
        lineCount++;
    }

    printf("\n--- Optimized TAC ---\n");
    for(int i=0;i<lineCount;i++) {
        if(sscanf(input[i], " %s = %s %c %s", result, op1, &op, op2) == 4) {
            optimizeTAC(result, op1, op, op2);
        } else if(sscanf(input[i], " %s = %s", result, op1) == 2) {
            optimizeTAC(result, op1, '\0', "");
        } else {
            printf("Invalid TAC format: %s\n", input[i]);
        }
    }

    return 0;
}
