#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct Symbol {
    char* name;
    int address;
    int is_data;
    int is_extern;
    int is_entry;
    struct Symbol* next;
} Symbol;
 
/*זה בעצם המסנה של הטבלת סמלים שמאפשר גישה באו של 1 ולא באו של אן*/
typedef struct {                 
    Symbol *cells[SYMBOL_TABLE_SIZE];
} SymbolTable;

extern Symbol* symbol_table_head;

/* functions */
void add_symbol(char* name, int address, int is_data, int is_extern);
Symbol* find_symbol(const char* name);
void report_error(const char* message, int line_num);
void mark_entry(const char* name);
void free_symbol_table(void);

/* NNEDS TO BE WRITTEN LATER!!!*/
SymbolTable* create_symbol_table(void);
void add_to_hash_table(SymbolTable* table, char* name, int address, int is_data, int is_extern);
Symbol* find_in_hash_table(SymbolTable* table, const char* name);
void free_hash_table(SymbolTable* table);

#endif /* SYMBOL_TABLE_H */
