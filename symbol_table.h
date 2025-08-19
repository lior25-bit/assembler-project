#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/* Headers needed for compilation */
#include "constants.h"

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
    int is_entry;
    struct Symbol* next; /* serves as a linked list */ 
} Symbol;

/* The SymbolTable struct holds the "head" of the table and each symbol points to the next symbol in the list
   as part of his structure */
typedef struct {
    Symbol* head;
    Symbol* last;
} SymbolTable;

/* functions */
/* building functions */
Symbol* create_symbol(const char* name, int address, SymbolType type);
SymbolTable* create_empty_table();

/* validations */
int is_valid_symbol(Symbol* symbol, SymbolTable* table);
int is_valid_table(SymbolTable* table);
int is_valid_symbol_name(const char* name);
int is_empty_table(SymbolTable* table);
int is_reserved_word(const char* name);
int is_valid_char_symbol_name(const char* name);
int is_valid_address(int address);
int is_valid_symbol_type(SymbolType type);


/* helpers */
Symbol* find_symbol(SymbolTable* table, const char* name);
SymbolTable* add_symbol_head(SymbolTable* table, Symbol* head); 
SymbolTable* add_last_symbol(SymbolTable* table, Symbol* last);
void mark_entry(SymbolTable* table, const char* name);
int is_entry_symbol(Symbol* symbol);
void update_data_symbols(SymbolTable* table, int ic);
int get_symbol_count(SymbolTable* table); 

/* freeing functions */
void free_symbol_table(SymbolTable* table);

#endif
