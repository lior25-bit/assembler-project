/* This file contains the functions that takes care of the logical aspects of processing symbols. */

#ifndef SYMBOL_LOGIC_H
#define SYMBOL_LOGIC_H

#include "error_manager.h"

/* Functions */
int add_symbol_cheaked(SymbolTable* table, const char* name, int address, SymbolType type, ErrorManager* error_mgr, int line);
int mark_entry_checked(SymbolTable* table, const char* name, ErrorManager* error_mgr, int line);


#endif
