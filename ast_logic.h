/* This file contains the functions that are used to handle the logical aspect of AST Nodes processing */

#ifndef AST_LOGIC_H
#define AST_LOGIC_H

#include "ast.h"
#include "constants.h"
#include <string.h>

/* validation of node types */
int is_directive_node(ASTNode* node);
int is_instruction_node(ASTNode* node);
int is_extern_directive_node(ASTNode* node);
int is_entry_directive_node(ASTNode* node);
int is_data_directive_node(ASTNode* node);
int is_string_directive_node(ASTNode* node);
int is_mat_directive_node(ASTNode* node);

/* Helpers - Opcodes */
int is_directive_opcode(OpCode op); 
int is_zero_op_instruction(OpCode op);
int is_one_op_instruction(OpCode op);
int is_two_op_instruction(OpCode op);

/* nodes calculations and cheaking functions */
int has_label_node(ASTNode* node);
int has_data_node(ASTNode* node);
int calculate_memory_size_node(ASTNode* node);
int count_node_operand(ASTNode* node);
int get_operand_count(OpCode op);

#endif
