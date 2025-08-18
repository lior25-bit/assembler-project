#include "symbol_table.h"
#include "symbol_logic.h"
/* This function add an exsiting symbol to a given symbol table only after validating it and handelling errors. */
int add_symbol_cheaked(SymbolTable* table, const char* name, int address, SymbolType type, ErrorManager* error_mgr, int line) {
  if(!table || !name || !address || !type || !line) {
    add_error(error_mgr,ALLOC_FAILED,0);
    return 0;
  }
  if(!error_mgr) {
    failed_alloc_error_mgr();
    return 0;
}
Symbol symbol = find_symbol(table,name);
}

/* This functions marks a symbol as entry (if applicable) only after validating it and handelling errors. */
int mark_entry_checked(SymbolTable* table, const char* name, ErrorManager* error_mgr, int line) {
}


