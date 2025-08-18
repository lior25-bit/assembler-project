#include "error_manager.h"
#include "symbol_table.h" 
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

/* initialize the error manager - set count to zero */
void initialize_error_manager(ErrorManager* mgr) {
    if (!mgr) {
        printf("ERROR: ErrorManager is NULL\n");
        return;
    }
    mgr->count = 0;  
}

/* this function checks if a symbol is valid */
int isValid_symbol(Symbol* symbol, ErrorManager* error_mgr, int line) {
    if (!is_error_mgr_initialized(error_mgr)) {
        return 0;
    }    
    if (!symbol) {
        add_error(error_mgr, SYMBOL_HEAD_NOT_ALLOC, line);
        return 0;
    }
    return 1;
}

/* check if the error manager itself is valid */
int isValid_error_manager(ErrorManager* error_mgr) {
    if (!is_error_mgr_initialized(error_mgr)) {
        return 0;
    }
    if (error_mgr->count < 0 || error_mgr->count > MAX_ERROR) {
        return 0;
    }
    return 1;
}

/* this function adds a new error to the error manager */
void add_error(ErrorManager* error_mgr, ErrorName name, int line) {
    char* message; /* we need to declare variables at the start for C90 */
    
    if (!is_error_mgr_initialized(error_mgr)) {
        return;
    }
    if (!isValid_error_name(name)) {
        return;
    }
    if (!isValid_error_line(line)) {
        return;
    }
    if (error_mgr->count >= MAX_ERROR) {
        printf("ERROR: Cannot add more errors - limit reached\n");
        return;
    }
    
    /* fill in the error details */
    error_mgr->list[error_mgr->count].name = name;
    error_mgr->list[error_mgr->count].line = line;
    
    /* get the error message and copy it */
    message = get_error_message(name);
    strcpy(error_mgr->list[error_mgr->count].message, message);
    
    /* increment the error count */
    error_mgr->count++;
}

/* check if the error manager was initialized properly */
int is_error_mgr_initialized(ErrorManager* error_mgr) {
    if (!error_mgr) {
        printf("ERROR: ErrorManager not initialized.\n");
        return 0;
    }
    return 1;
}

/* returns 1 if there are errors, 0 if no errors found */
int has_errors(ErrorManager* error_mgr) {
    if (!is_error_mgr_initialized(error_mgr)) {
        return 0;
    }
    return (error_mgr->count > 0);
}

/* print all the errors that were found during assembly */
void print_all_errors(ErrorManager* error_mgr) {
    int i; /* declare loop variable at start for C90 */
    
    if (!is_error_mgr_initialized(error_mgr)) {
        return;
    }
    
    /* go through all errors and print them */
    for (i = 0; i < error_mgr->count; i++) {
        printf("Line %d: %s\n", error_mgr->list[i].line, error_mgr->list[i].message);
    }
}

/* clear all errors from the manager - reset count to zero */
void clear_errors(ErrorManager* error_mgr) {
    if (!is_error_mgr_initialized(error_mgr)) {
        return;
    }
    error_mgr->count = 0;
}

/* check if the error name is within valid range */
int isValid_error_name(ErrorName name) {
    return (name >= ALLOC_FAILED && name <= INVALID_ADDRESS);
}

/* check if line number is valid (positive and not too big) */
int isValid_error_line(int line) {
    return (line > 0 && line <= MAX_LINE);
}

/* check if error message string is valid */
int isValid_error_message(const char* message) {
    if (!message) {
        return 0;
    }
    return (strlen(message) < MAX_MESSAGE);
}

/* print error message when memory allocation fails */
void failed_alloc_error_mgr(void) {
    printf("Error: Error Manager has failed to allocate memory\n");
}

/* this function returns the appropriate error message for each error type */
char* get_error_message(ErrorName name) {
    switch (name) {
        case ALLOC_FAILED: 
            return "Memory allocation failed";
        case TABLE_NOT_ALLOC: 
            return "Table not allocated";
        case SYMBOL_HEAD_NOT_ALLOC: 
            return "Symbol head not allocated";
        case SYMBOL_ALREADY_DEFINED: 
            return "Symbol already defined";
        case SYMBOL_NO_EXIST: 
            return "Symbol does not exist";
        case INVALID_OPCODE: 
            return "Invalid opcode";
        case UNKNOWN_OPCODE: 
            return "Unknown opcode";
        case EXTERN_LABEL: 
            return "Extern label error";
        case ENTRY_LABEL: 
            return "Entry label error";
        case EXTERN_AND_ENTRY_LABEL: 
            return "Extern and entry label conflict";
        case INVALID_ADDRESSING_MODE: 
            return "Invalid addressing mode";
        case MEMORY_OVERFLOW: 
            return "Memory overflow";
        case INVALID_ADDRESS: 
            return "Invalid address";
        default: 
            return "Unknown error";
    }
}
