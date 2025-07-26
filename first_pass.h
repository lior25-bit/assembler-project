#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "ast.h"
#include "symbol_table.h"

/* קבועים */
#define MEMORY_START 100
#define MAX_MEMORY 4096

/* מבנה לניהול הזיכרון במהלך המעבר הראשון */
typedef struct {
    int IC; /* מונה הוראות */
    int DC; /* מונה נתונים */
    int code_image[MAX_MEMORY]; /* הוראות */
    int data_image[MAX_MEMORY]; /* נתונים */
} MemoryManager;

/* הצהרות לפונקציות המעבר הראשון */
int first_pass(ASTNode* ast_head, SymbolTable* symbols, MemoryManager* memory);
int process_node(ASTNode* node, SymbolTable* symbols, MemoryManager* memory);
int is_directive(ASTNode* node);
int is_extern_directive(ASTNode* node);
int calculate_instruction_size(ASTNode* node);

#endif /* FIRST_PASS_H */
