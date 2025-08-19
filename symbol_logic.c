/* This file contains the functions that takes care of the logical aspects of processing symbols. */

#include "symbol_logic.h"
#include <string.h>

/* This function cheaks if a symbol can be added to a given symbol table */
int symbol_cheaked(SymbolTable* table, const char* name, int address, SymbolType type, ErrorManager* error_mgr) {

    if(!error_mgr) {
        return 0;
    }
    if(!table) {
        add_error(error_mgr, TABLE_NOT_ALLOC, 0);
        return 0;
    }
    if(!name || !name[0]) {
        add_error(error_mgr, INVALID_SYMBOL_NAME, 0);
        return 0;
    }
    if(!is_valid_symbol_name(name)) {
        if(is_reserved_word(name)) {
            add_error(error_mgr, RESERVED_WORD_SYMBOL, 0);
        } else {
            add_error(error_mgr, INVALID_SYMBOL_NAME, 0);
        }
        return 0;
    }
    if(!is_valid_address(address)) {
        add_error(error_mgr, INVALID_ADDRESS, 0);
        return 0;
    }
    if(find_symbol(table, name)) {
        add_error(error_mgr, SYMBOL_ALREADY_DEFINED, 0);
        return 0;
    }
    if(!is_valid_symbol_type(type)) {
        add_error(error_mgr, INVALID_SYMBOL_TYPE, 0);
        return 0;
    }
    return 1;
}

/* This functions marks a symbol as entry (if applicable) only after validating it and handelling errors. */
int mark_entry_checked(SymbolTable* table, const char* name, ErrorManager* error_mgr) {
    Symbol* sym;

    if (!error_mgr) {
        return 0;
    }
    if (!table) {
        add_error(error_mgr, TABLE_NOT_ALLOC, 0);
        return 0;
    }
    if (!name || !name[0]) {
        add_error(error_mgr, INVALID_SYMBOL_NAME, 0);
        return 0;
    }

    sym = find_symbol(table, name);
    if (!sym) {
        add_error(error_mgr, SYMBOL_NO_EXIST, 0);
        return 0;
    }
    if (sym->type == SYMBOL_EXTERN) {
        add_error(error_mgr, EXTERN_LABEL, 0);
        return 0;
    }
    if (sym->is_entry) {
        return 1; /* already an entry – success */
    }

    sym->is_entry = 1;
    return 1;
}
