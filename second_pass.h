#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "ast.h"
#include "symbol_table.h"
#include "error_manager.h"
#include "memory_manager.h"

/* Main functions */
int second_pass(ASTNode* ast_head, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr);
int process_node_second_pass(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr);

/* Directive processing functions */
int process_directive_second_pass(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr);
int handle_entry_directive(ASTNode* node, SymbolTable* table, ErrorManager* error_mgr);
int encode_data_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr);
int encode_string_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr);
int encode_mat_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr);

/* Instruction encoding */
int process_instruction_second_pass(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr);
int encode_instruction(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr);
int encode_operand(Operand operand, MemoryManager* memory, SymbolTable* table, int is_source, ErrorManager* error_mgr);

/* Symbol functions */
int get_symbol_address(SymbolTable* table, char* symbol_name);
int validate_symbol_exists(SymbolTable* table, char* symbol_name, ErrorManager* error_mgr, int line);

/* calculation helpers functions */
int create_machine_word(int opcode, int src_mode, int dest_mode, int are);
void write_word_to_memory(MemoryManager* memory, int address, int word, int are);
int get_addressing_mode_code(ArgType type);
int calculate_operand_words(Operand operand);


#endif
