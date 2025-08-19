/* This header manages the file exportation */
#ifndef FILE_EXPORT_H
#define FILE_EXPORT_H

#include "memory_manager.h"
#include "symbol_table.h"
#include "ast.h"
#include "constants.h"

/* Main export functions */
int export_ob_file(char* file_name, MemoryManager* memory);
int export_ent_file(char* file_name, SymbolTable* table);
int export_ext_file(char* file_name, MemoryManager* memory);
int export_am_file(char* file_name, ASTNode* ast_head);
int export_all_files(char* file_name, MemoryManager* memory, SymbolTable* table, ASTNode* expanded_ast);

/* Helper functions */
char* create_file_name(char* file_name, char* output_type);
char* convert_to_base_four(int value);
int has_entry_symbols(SymbolTable* table);
int has_extern_references(MemoryManager* memory);

#endif
