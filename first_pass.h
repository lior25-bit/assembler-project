#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "ast.h"
#include "symbol_table.h"
#include <string.h>
#include <ctype.h>

/* קבועים */
#define MEMORY_START 100
#define MAX_MEMORY 4096
#define MAX_LABEL 30

/* מבנה לניהול הזיכרון במהלך המעבר הראשון */
typedef struct {
    int IC; /* מונה הוראות */
    int DC; /* מונה נתונים */
    int code_image[MAX_MEMORY]; /* הוראות */
    int data_image[MAX_MEMORY]; /* נתונים */
} MemoryManager;

int first_pass(ASTNode* ast_head, MemoryManager* memory);
int process_node(ASTNode* node, MemoryManager* memory);

int is_directive(ASTNode* node);
int is_extern_directive(ASTNode* node);
int is_valid_label(const char* label);
int is_reserved_word(const char* name);
int process_label(ASTNode* node);
SymbolType process_node_type(ASTNode* node);
int calculate_instruction_size(ASTNode* node);

void update_data_symbols(Symbol* head, int ic);

#endif /* FIRST_PASS_H */


