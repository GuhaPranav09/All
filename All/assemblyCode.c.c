#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_REGISTERS 4
#define MAX_LINES 100

int registers[MAX_REGISTERS] = {0};

// Allocate a free register (R0-R3)
int allocateRegister() {
    for (int i = 0; i < MAX_REGISTERS; i++) {
        if (registers[i] == 0) {
            registers[i] = 1;
            return i;
        }
    }
    printf("Error: Out of registers\n");
    return -1;
}

// Free a register
void freeRegister(int reg) {
    if (reg >= 0 && reg < MAX_REGISTERS)
        registers[reg] = 0;
}

// Generate assembly for a TAC statement
void generateAssembly(char* result, char* op1, char op, char* op2) {
    int reg1, reg2;

    if (op == '\0') {  // simple assignment
        reg1 = allocateRegister();
        printf("MOV R%d, %s\n", reg1, op1);
        printf("MOV %s, R%d\n", result, reg1);
        freeRegister(reg1);
        return;
    }

    reg1 = allocateRegister();
    reg2 = allocateRegister();
    printf("MOV R%d, %s\n", reg1, op1);
    printf("MOV R%d, %s\n", reg2, op2);

    switch (op) {
        case '+': printf("ADD R%d, R%d\n", reg1, reg2); break;
        case '-': printf("SUB R%d, R%d\n", reg1, reg2); break;
        case '*': printf("MUL R%d, R%d\n", reg1, reg2); break;
        case '/': printf("DIV R%d, R%d\n", reg1, reg2); break;
        case '<': printf("SHL R%d, R%d\n", reg1, reg2); break;  // << operator
        default: printf("Invalid operator\n"); freeRegister(reg1); freeRegister(reg2); return;
    }

    printf("MOV %s, R%d\n", result, reg1);
    freeRegister(reg1);
    freeRegister(reg2);
}

int main() {
    char input[MAX_LINES][30];
    char result[10], op1[10], op2[10], op;
    int lineCount = 0;

    printf("Enter three-address code statements (type 'exit' to stop):\n");
    while (1) {
        fgets(input[lineCount], sizeof(input[lineCount]), stdin);
        input[lineCount][strcspn(input[lineCount], "\n")] = '\0';
        if (strcmp(input[lineCount], "exit") == 0)
            break;
        lineCount++;
    }

    printf("\n\n");

    for (int i = 0; i < lineCount; i++) {
        if (sscanf(input[i], " %s = %s %c %s", result, op1, &op, op2) == 4) {
            generateAssembly(result, op1, op, op2);
        } else if (sscanf(input[i], " %s = %s", result, op1) == 2) {
            generateAssembly(result, op1, '\0', "");
        } else {
            printf("Invalid format in line: %s\n", input[i]);
        }
    }

    return 0;
}
