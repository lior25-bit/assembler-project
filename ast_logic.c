/* This file contains the AST logic functions for the assembler */

#include "ast_logic.h"
#include <string.h>

/* This functions cheaks if a given node is directive */
int is_directive_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    return (node->node_type == NODE_DIRECTIVE);
}

/* This function cheaks if a given node is instruction */
int is_instruction_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    return (node->node_type == NODE_INSTRUCTION);
}

/* This function cheaks if a given node is extern directive */
int is_extern_directive_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    return (node->node_type == NODE_DIRECTIVE &&
            node->directive.dir_type == DIR_EXTERN);
}

/* This function cheaks if a given node is entry directive */
int is_entry_directive_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    return (node->node_type == NODE_DIRECTIVE &&
            node->directive.dir_type == DIR_ENTRY);
}

/* This function cheaks if a given node is data directive */
int is_data_directive_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    return (node->node_type == NODE_DIRECTIVE &&
            node->directive.dir_type == DIR_DATA);
}

/* This function cheaks if a given node is string directive */
int is_string_directive_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    return (node->node_type == NODE_DIRECTIVE &&
            node->directive.dir_type == DIR_STRING);
}

/* This function cheaks if a given node is matrix directive */
int is_mat_directive_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    return (node->node_type == NODE_DIRECTIVE &&
            node->directive.dir_type == DIR_MAT);
}

/* This function cheaks if a given opcode is a directive */
int is_directive_opcode(OpCode op) {
    (void)op;
    return 0;
}

/* This function cheaks if a given opcode is a zero operand instruction */
int is_zero_op_instruction(OpCode op) {
    return (op == OP_RTS || op == OP_STOP);
}

/* This function cheaks if a given opcode is a one operand instruction */
int is_one_op_instruction(OpCode op) {
    /* OP_CLR..OP_PRN */
    return (op >= OP_CLR && op <= OP_PRN);
}

/* This function cheaks if a given opcode is a two operand instruction */
int is_two_op_instruction(OpCode op) {
    /* OP_MOV..OP_LEA */
    return (op >= OP_MOV && op <= OP_LEA);
}

/* This function cheaks if a given node has a label */
int has_label_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    return (node->label != 0);
}

/* This function cheaks if a given node is has data */
int has_data_node(ASTNode* node) {
    if (!node) {
        return 0;
    }
    if (!is_directive_node(node)) {
        return 0;
    }
    return (node->directive.dir_type == DIR_DATA ||
            node->directive.dir_type == DIR_STRING ||
            node->directive.dir_type == DIR_MAT);
}

/* This function calculates the "size" a given node occupies in the memory */
int calculate_memory_size_node(ASTNode* node) {
    if (!node) {
        return 0;
    }

    if (is_instruction_node(node)) {
        return get_operand_count(node->opcode) + 1; /* מילה ראשית */
    }

    if (is_directive_node(node)) {
        switch (node->directive.dir_type) {
        case DIR_DATA:
            return node->directive.value.data.count;
        case DIR_STRING:
            /* כולל '\0' */
            return (node->directive.value.string_val != 0) ?
                   (int)strlen(node->directive.value.string_val) + 1 : 1;
        case DIR_MAT:
            return node->directive.value.mat.rows *
                   node->directive.value.mat.cols;
        case DIR_EXTERN:
        case DIR_ENTRY:
        default:
            return 0;
        }
    }

    return 0;
}

/* This function counts the number of operands in a given node */
int count_node_operand(ASTNode* node) {
    int count;
    if (!node) {
        return 0;
    }
    count = 0;
    if (node->operands[0].type != ARG_NONE) count++;
    if (node->operands[1].type != ARG_NONE) count++;
    return count;
}

int get_operand_count(OpCode op) {
    if (is_zero_op_instruction(op)) {
        return 0;
    }
    if (is_one_op_instruction(op)) {
        return 1;
    }
    if (is_two_op_instruction(op)) {
        return 2;
    }
    return 0;
}
