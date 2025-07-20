#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "ast.h"
#include "frontend_error.h"
#include "symbol_table.h"

/* CONSTANTS */
#define MEMORY_START 100
#define MAX_MEMORY 4096 /*לפי הצאט זה האולטימטיבי אסביר לך למה או שתשאלי את הצאט*/

typedef struct { 
    int IC; /*מונה הוראות*/
    int DC; /*מונה נתונים */
    int code_image[MAX_MEMORY]; /*מערך של כל ההוראות*/
    int data_image[MAX_MEMORY]; /*מערך של הנתונים*/
} MemoryManager; /* מבנה הנתונים של מנהל הזיכרון*/

/* פונקציות - הצהרות*/

MemoryManager* create_memory_manager();
int first_pass(ASTNode* ast_head, SymbolTable* symbols, MemoryManager* memory); /*מבצעת את המעבר הראשון על כל הAST*/
int process_node(ASTNode* node, SymbolTable* symbols, MemoryManager* memory);
int process_instruction(ASTNode* node, SymbolTable* symbols, MemoryManager* memory);
int process_directive(ASTNode* node, SymbolTable* symbols, MemoryManager* memory);
int calculate_instruction_size(ASTNode* node);
void init_memory_manager(MemoryManager* memory);




