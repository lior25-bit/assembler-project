/* This file contains all the necassary function for symbol table management */

#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* This function creates a new symbol*/
Symbol* create_symbol(const char* name, int address, SymbolType type) {
    Symbol* symbol = (Symbol*)calloc(1, sizeof(Symbol));
    if(!symbol) {
       return NULL;
    }
    /* symbol was allocated successfully */
    strncpy(symbol->name, name ? name : "", MAX_SYMBOL_NAME - 1);
    symbol->name[MAX_SYMBOL_NAME - 1] = '\0';
    symbol->address = address;
    symbol->type = type;
    symbol->is_entry = 0;
    symbol->next = NULL;
    return symbol;
}

/* This function creates a new empty symbol table */
SymbolTable* create_empty_table() {
    SymbolTable* table = (SymbolTable*)calloc(1, sizeof(SymbolTable));
    return table;
}

int is_valid_table(SymbolTable* table) {
    Symbol* i;
    Symbol* j;
    if(!table) {
        return 0;
    }
    for (i = table->head; i != NULL; i = i->next) {
        if(!is_valid_symbol(i, table)) {
            return 0;
        }
        for (j = i->next; j != NULL; j = j->next) {
            if (strcmp(i->name, j->name) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

/* This function checks if a symbol is valid for a given symbol table */
int is_valid_symbol(Symbol* symbol, SymbolTable* table) {
    const char* name;
    int address;
    if(!symbol || !table) {
        return 0;
    }
    name = symbol->name;
    address = symbol->address;
    if(!is_valid_symbol_name(name)) {
        return 0;
    }
    if(!is_valid_address(address)) {
        return 0;
    }
    if(symbol->type == SYMBOL_EXTERN && symbol->is_entry == 1) {
        return 0;
    }
    return 1;
}

/* This function checks if a symbol name is valid */
int is_valid_symbol_name(const char* name) {
    if(!name) {
        return 0;
    }
    if(strlen(name) >= MAX_SYMBOL_NAME) {
        return 0;
    }
    if(!isalpha((unsigned char)name[0])) {
        return 0;
    }
    if(is_reserved_word(name)) {
        return 0;
    }
    if(!is_valid_char_symbol_name(name)) {
        return 0;
    }
    return 1;
}

/* This function checks if a symbol name is a reserved word */
int is_reserved_word(const char* name) {
    const char* reserved_list[] = {
        "mov", "cmp", "add", "sub", "lea",
        "clr", "not", "inc", "dec",
        "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
        "data", "string", "mat", "entry", "extern",
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
    };
    int count;
    int i;
    count = (int)(sizeof(reserved_list) / sizeof(reserved_list[0]));
    for(i = 0; i < count; i++) {
        if(strcmp(name, reserved_list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* This funcition cheaks if a symbol name consists only valid chars  */
int is_valid_char_symbol_name(const char* name) {
    int i;
    if(!name) {
        return 0;
    }
    for(i = 1; name[i] != '\0'; i++) {
        if(!isalnum((unsigned char)name[i])) {
            return 0;
        }
    }
    return 1;
}

/* This function cheaks if a symbol address is valid */
int is_valid_address(int address) {
    return (address >= MIN_ADDRESS && address <= MAX_ADDRESS);
}

/* This function cheaks if a symbol table is empty */
int is_empty_table(SymbolTable* table) {
    return (!table || table->head == NULL);
}

/* This function finds a symbol in a given symbol table */
Symbol* find_symbol(SymbolTable* table, const char* name) {
    Symbol* current;
    if(!table || !name) {
        return NULL;
    }
    current = table->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

/* This function mark entry for a symbol in a given symbol table if applicable */
void mark_entry(SymbolTable* table, const char* name) {
    Symbol* symbol = find_symbol(table, name);
    if (symbol) {
        symbol->is_entry = 1;
    }
}

int is_entry_symbol(Symbol* symbol) {
    return (symbol && symbol->is_entry);
}

/* This function adds a symbol to the head of a given symbol table */
SymbolTable* add_symbol_head(SymbolTable* table, Symbol* head) {
    if(!table || !head) {
        return NULL;
    }
    head->next = table->head;
    table->head = head;
    if(!table->last) {
        table->last = head;
    }
    return table;
}

/* This function adds a symbol to the end of a given symbol table */
SymbolTable* add_last_symbol(SymbolTable* table, Symbol* last) {
    if(!table || !last) {
        return NULL;
    }
    if(table->last == NULL) {
        table->head = last;
        table->last = last;
    } else {
        table->last->next = last;
        table->last = last;
    }
    last->next = NULL;
    return table;
}

/* This function updates data symbols in a given symbol table */
void update_data_symbols(SymbolTable* table, int ic) {
    Symbol* current;
    if(!table) {
        return;
    }
    current = table->head;
    while(current != NULL ) {
        if(current->type == SYMBOL_DATA)  {
            current->address += ic;
        }
        current = current->next;
    }
}

/* This function free the memory occupied by a given symbol table */
void free_symbol_table(SymbolTable* table) {
    Symbol* current;
    if(!table) {
        return;
    }
    current = table->head;
    while (current != NULL) {
        Symbol* temp = current;
        current = current->next;
        free(temp);
    }
    table->head = NULL;
    table->last = NULL;
    free(table);
}

/* This function counts the number of symbols in a given symbol table */
int get_symbol_count(SymbolTable* table) {
    Symbol* current;
    int count;
    if(!table) {
        return 0;
    }
    current = table->head;
    count = 0;
    while(current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

/* This function cheaks if a symbol type is valid */
int is_valid_symbol_type(SymbolType type) {
    return (type == SYMBOL_CODE || type == SYMBOL_DATA || type == SYMBOL_EXTERN); /* SYMBOL_NONE is considered invalid for table insertion */
}
