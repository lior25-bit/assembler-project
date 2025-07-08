/* file: ast.c
   description: AST code with functions used for building ast syntax tree.
   by: Lir Vine and Lior Tal.

IMPORTANT 
 This act.c has operand building functions, actNode building function and free functions and print function for debug purpuses 
 The strings are being saved directly in act.c - in the future we might want to save it in symbol or string table - we need to see. for now this is good 
 */

#include "ast.h"
#include <stdlib.h>   /* malloc, free */
#include <string.h>   /* strdup */
#include <stdio.h>    

Operand new_immediate(int val) {
    Operand op;
    op.type = ARG_IMMEDIATE;
    op.value = val;
    return op;
}

Operand new_register(int val) {
    Operand op;
    op.type = ARG_REGISTER;
    op.reg_value = val;
    return op;
}

Operand new_label(char* input) {
    Operand op;
    op.type = ARG_DIRECT;
    op.label = input;
    return op;
}

Operand new_matrix(int row_reg, int col_reg) {
    Operand op;
    op.type = ARG_MATRIX;
    op.matrix_regs[0] = row_reg;
    op.matrix_regs[1] = col_reg;
    return op;
}

Operand empty_operand() {
    Operand op;
    op.type = ARG_NONE;
    return op;
}

ASTNode* new_node(OpCode op, Operand op1, Operand op2, int new_address, char* new_label, char* og_line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->opcode = op;
    node->operands[0] = op1;
    node->operands[1] = op2;
    node->address = new_address;
    node->label = strdup(new_label);
    node->original_line = strdup(og_line);
    return node;
}

/* This function frees the memory inside Operand if needed */
void free_operand(Operand* op) {
    if (!op) return;

    if (op->type == ARG_DIRECT && op->label != NULL) {
        free(op->label);
        op->label = NULL;
    }

    op->type = ARG_NONE;
}

void free_label(ASTNode* node) {
    if (node->label != NULL)
        free(node->label);
}

void free_node(ASTNode* node) {
    if (!node) return;

    free_label(node);
    free_operand(&node->operands[0]);
    free_operand(&node->operands[1]);

    if (node->original_line != NULL)
        free(node->original_line);

    free(node);
}

void print_operand(Operand op) {
    switch (op.type) {
        case ARG_IMMEDIATE:
            printf("Immediate: #%d\n", op.value);
            break;
        case ARG_REGISTER:
            printf("Register: R%d\n", op.reg_value);
            break;
        case ARG_DIRECT:
            printf("Label: %s\n", op.label);
            break;
        case ARG_MATRIX:
            printf("Matrix: [%d][%d]\n", op.matrix_regs[0], op.matrix_regs[1]);
            break;
        case ARG_NONE:
            printf("No operand\n");
            break;
        default:
            printf("Unknown operand type\n");
            break;
    }
}

void print_opcode(ASTNode* node) {
    switch (node->opcode) {
        case OP_MOV:  printf("MOV\n"); break;
        case OP_CMP:  printf("CMP\n"); break;
        case OP_ADD:  printf("ADD\n"); break;
        case OP_SUB:  printf("SUB\n"); break;
        case OP_LEA:  printf("LEA\n"); break;
        case OP_CLR:  printf("CLR\n"); break;
        case OP_NOT:  printf("NOT\n"); break;
        case OP_INC:  printf("INC\n"); break;
        case OP_DEC:  printf("DEC\n"); break;
        case OP_JMP:  printf("JMP\n"); break;
        case OP_BNE:  printf("BNE\n"); break;
        case OP_JSR:  printf("JSR\n"); break;
        case OP_RED:  printf("RED\n"); break;
        case OP_PRN:  printf("PRN\n"); break;
        case OP_RTS:  printf("RTS\n"); break;
        case OP_STOP: printf("STOP\n"); break;
        default:      printf("Unknown OpCode\n"); break;
    }
}

void print_label(ASTNode* node) {
    if (node->label != NULL)
        printf("%s\n", node->label);
    else
        printf("(no label)\n");
}

void print_ast_node(ASTNode* node) {
    printf("OpCode: ");
    print_opcode(node);

    printf("Operands:\n");
    print_operand(node->operands[0]);
    print_operand(node->operands[1]);

    printf("Label: ");
    print_label(node);

    printf("Address: %d\n", node->address);
    printf("Original line: %s\n", node->original_line);
}