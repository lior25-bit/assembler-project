#include "symbol_table.h"
#include "middle_error.h"


Symbol* symbol_table_head = NULL;

void add_symbol(char* name, int address, SymbolType type) {
    if(find_symbol(name) != NULL) {
        symbol_alrady_defiend_error(name);
        return;
    }

    Symbol* new_sym = malloc(sizeof(Symbol));
    new_sym->name = strdup(name);
    new_sym->address = address;
    new_sym->is_entry = 0;
    new_sym->type = type;

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

/* Updates every data type Symbol, The function gets the "size" the instructions occupied in the memory */
void adjust_data_symbols(int instructions_size) {
    Symbol* current = symbol_table_head;
    while(current != NULL ) {
        if(current->type == SYMBOL_DATA)  { 
            current->address += instructions_size;
        }
      current = current->next;
    }
 return;
}

void mark_entry(const char* name) {
    Symbol* sym = find_symbol(name);
    if (sym != NULL) {
        if (sym->type == SYMBOL_EXTERN) {
            entry_on_external_error(name); 
            return;
        }
        sym->is_entry = 1;
    }
}


void free_symbol_table() {
    Symbol* current = symbol_table_head;
    while (current != NULL) {
        Symbol* temp = current;
        current = current->next;
        free(temp->name);
        free(temp);
    }
    symbol_table_head = NULL;
}

/* used for debug purposes */
void print_symbol_table() {
    Symbol* current = symbol_table_head;
    while (current != NULL) {
        printf("Name: %s\n", current->name);

        if (current->type == SYMBOL_DATA) {
            printf("Type: DATA\n");
        } else {
            printf("Type: CODE\n");
        }

        if (current->is_entry) {
            printf("Symbol is entry\n");
        } else {
            printf("Symbol is not entry\n");
        }

        if (current->type == SYMBOL_EXTERN) {
            printf("Symbol is extern\n");
        } else {
            printf("Symbol is not extern\n");
        }

        printf("------------------------\n"); 
        current = current->next;
    }
}
