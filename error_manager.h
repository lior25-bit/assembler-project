/* This file is the header that handles error management for the assembler. */

#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include "constants.h"

typedef enum {
    ALLOC_FAILED,
    TABLE_NOT_ALLOC,
    SYMBOL_ALREADY_DEFINED,
    SYMBOL_NO_EXIST,
    INVALID_SYMBOL_NAME,
    INVALID_SYMBOL_TYPE,
    RESERVED_WORD_SYMBOL,
    INVALID_OPCODE,            
    UNKNOWN_OPCODE,
    EXTERN_LABEL,
    ENTRY_LABEL,
    EXTERN_AND_ENTRY_LABEL,
    INVALID_ADDRESSING_MODE,
    MEMORY_OVERFLOW,
    INVALID_ADDRESS 
} ErrorName;

/* struct of a single error */
typedef struct { 
    ErrorName name;
    char message[MAX_MESSAGE];
    char original_line[MAX_LINE]; /* reference to original error line */
}Error;

/*struct of a error manager - contains data on all errors of a certain AST node. */
typedef struct {
    Error list[MAX_ERROR];
    int count;
}ErrorManager;

/* Functions Declarations */

/* Main Functions */
void initialize_error_manager(ErrorManager* mgr);
int  has_errors(ErrorManager* mgr);
void clear_errors(ErrorManager* mgr);
void print_all_errors(ErrorManager* mgr);
void add_error(ErrorManager* mgr, ErrorName name, const char* og_line);
const char* get_error_message(ErrorName name);

#endif 
