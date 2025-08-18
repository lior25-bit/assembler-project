#ifndef FIRST_PASS_H
#define FIRST_PASS_H

/* Heathers needed for compilation */
#include "error_manager.h"
#include "memory_manager.h"
#include "symbol_table.h"
#include "ast.h"


/* Function Declarations */

/* Main functions */
int first_pass(ASTNode* ast_head, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr);
int process_node(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr);

/* Helpers */
int process_instruction_node(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr);
int process_directive_node(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr);
SymbolType process_node_type(ASTNode* node);
int is_valid_addressing_mode(OpCode op, ArgType src, ArgType dest, ErrorManager* error_mgr, int line);
int validate_basic_node_params(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr);
int validate_full_node_params(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr);
int handle_node_label(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr);
int calculate_instruction_size(ASTNode* node, ErrorManager* error_mgr);
int calculate_matrix_memory(int rows, int cols);

#endif




