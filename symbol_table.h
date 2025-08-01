#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H


typedef enum {
    SYMBOL_CODE,  /* instruction */
    SYMBOL_DATA,  /* directive */
    SYMBOL_EXTERN /* extern */
} SymbolType;

typedef struct Symbol {
    char* name;
    int address;
    int is_entry;
    SymbolType type;
    struct Symbol* next; 
} Symbol;
 
extern Symbol* symbol_table_head;

/* functions */
void add_symbol(char* name, int address, SymbolType type);
Symbol* find_symbol(const char* name);
void mark_entry(const char* name);
void free_symbol_table();
void print_symbol_table();
void adjust_data_symbols(int instructions_size);



#endif /* SYMBOL_TABLE_H */
