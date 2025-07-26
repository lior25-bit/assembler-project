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
 
extern Symbol* symbol_table_head;

/* functions */
void add_symbol(char* name, int address, int is_data, int is_extern);
Symbol* find_symbol(const char* name);
void mark_entry(const char* name);
void free_symbol_table(void);
void print_symbol_table(void);



#endif /* SYMBOL_TABLE_H */
