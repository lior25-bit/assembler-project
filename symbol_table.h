#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "constants.h"
#include "ast.h"  
#include "error_manager.h" 

typedef enum {
    SYMBOL_CODE,  /* instruction */
    SYMBOL_DATA,  /* directive */
    SYMBOL_EXTERN, /* extern */
    SYMBOL_NONE /* NULL */
} SymbolType;

typedef struct Symbol {
    char name[MAX_SYMBOL_NAME];
    int address;
    SymbolType type;
    struct Symbol* next; /* serves as a linked list */ 
} Symbol;

/* The SymbolTable struct holds the "head" of the table and each symbol points to the next symbol in the list
   as part of his stucture */
typedef struct {
    Symbol* head;
    Symbol* last;
} SymbolTable;

/*functions*/
/* building functions */
Symbol* create_symbol(const char* name, int address, SymbolType type);
SymbolTable* create_empty_table();

/* validations */
int isValid_table(SymbolTable* table);
int is_valid_symbol_name(const char* name);

/* helpers */
SymbolType process_node_type(ASTNode* node, ErrorManager* error_mgr);
Symbol* find_symbol(SymbolTable* table, const char* name);
SymbolTable* add_head_symbol(SymbolTable* table, Symbol* head); 
SymbolTable* add_last_symbol(SymbolTable* table, Symbol* last);
void mark_entry(SymbolTable* table, const char* name);
void update_data_symbols(SymbolTable* table, int ic);
int get_symbol_count(SymbolTable* table); 

/* freeing functions */
void free_symbol_table(SymbolTable* table);
                


#endif 

               

