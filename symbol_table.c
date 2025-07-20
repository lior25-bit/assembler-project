#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Symbol* symbol_table_head = NULL;

void add_symbol(char* name, int address, int is_data, int is_extern) {
    Symbol* new_sym = malloc(sizeof(Symbol));
    new_sym->name = strdup(name);
    new_sym->address = address;
    new_sym->is_data = is_data;
    new_sym->is_extern = is_extern;
    new_sym->is_entry = 0;

    new_sym->next = symbol_table_head;
    symbol_table_head = new_sym;
}

Symbol* find_symbol(const char* name) {
    Symbol* current = symbol_table_head;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

void mark_entry(const char* name) {
    Symbol* sym = find_symbol(name);
    if (sym != NULL) {
        sym->is_entry = 1;
    }
}

void report_error(const char* message, int line_num) {
    fprintf(stderr, "Error on line %d: %s\n", line_num, message);
}

void free_symbol_table(void) {
    Symbol* current = symbol_table_head;
    while (current != NULL) {
        Symbol* temp = current;
        current = current->next;
        free(temp->name);
        free(temp);
    }
    symbol_table_head = NULL;
}
