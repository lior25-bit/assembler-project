#include "symbol_table.h"
#include "ast_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
    printf("symbol Table compiled successfully!\n");
    return 0;
}

Symbol* create_symbol(const char* name, int address, SymbolType type) {
    Symbol* symbol = calloc(1, sizeof(Symbol));
    if(!symbol) {
        return NULL;
    }
    strcpy(symbol->name, name);
    symbol->address = address;
    symbol->type = type;
    symbol->is_entry = 0;
    symbol->next = NULL;
    return symbol;
}

SymbolTable* create_empty_table() {
    SymbolTable* table = calloc(1, sizeof(SymbolTable));
    return table;
}

int is_valid_table(SymbolTable* table) {
    Symbol* current;
    if(!table) {
        return 0;
    }
    current = table->head;
    while(current != NULL) {
        if(!is_valid_symbol(current, table)) {
            return 0;
        }
        current = current->next;
    }
    return 1;
}

int is_valid_symbol(Symbol* Symbol, SymbolTable* table) {
    char* name;
    int address;
    if(!Symbol || !table) {
        return 0;
    }
    name = Symbol->name;
    address = Symbol->address;
    if(find_symbol(table, name)) {
        return 0;
    }
    if(!is_valid_symbol_name(name)) {
        return 0;
    }
    if(!is_valid_address(address)) {
        return 0;
    }
    if(Symbol->type == SYMBOL_EXTERN && Symbol->is_entry == 1) {
        return 0;
    }
    return 1;
}

int is_valid_symbol_name(const char* name) {
    if(!name) {
        return 0;
    }
    if(strlen(name) > MAX_SYMBOL_NAME) {
        return 0;
    }
    if(!isalpha(name[0])) {
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

int is_reserved_word(const char* name) {
    const char* reserved_list[] = {
        "mov", "cmp", "add", "sub", "lea",
        "clr", "not", "inc", "dec",
        "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
        "data", "string", "mat", "entry", "extern",
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
    };
    int count = (sizeof(reserved_list) / sizeof(reserved_list[0]));
    int i;
    for(i = 0; i < count; i++) {
        if(strcmp(name, reserved_list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_valid_char_symbol_name(const char* name) {
    int i;
    if(!name) {
        return 0;
    }
    for(i = 1; name[i] != '\0'; i++) {
        if(!isalnum(name[i])) {
            return 0;
        }
    }
    return 1;
}

int is_valid_address(int address) {
    return (address >= MIN_ADDRESS && address <= MAX_ADDRESS);
}

int is_empty_table(SymbolTable* table) {
    return (!table || table->head == NULL);
}

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

void mark_entry(SymbolTable* table, const char* name) {
    Symbol* symbol = find_symbol(table, name);
    if (symbol) {
        symbol->is_entry = 1;
    }
}

int is_entry_symbol(Symbol* symbol) {
    return (symbol && symbol->is_entry);
}

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
