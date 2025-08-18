#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H
#include "constants.h"
#include "sybmbol_table.h"

/* Error types that can occur during assembly */
typedef enum {
    ALLOC_FAILED,
    TABLE_NOT_ALLOC,
    SYMBOL_HEAD_NOT_ALLOC,
    SYMBOL_ALREADY_DEFINED,
    SYMBOL_NO_EXIST,
    INVALID_OPCODE,            
    UNKNOWN_OPCODE,
    EXTERN_LABEL,
    ENTRY_LABEL,
    EXTERN_AND_ENTRY_LABEL,
    INVALID_ADDRESSING_MODE,
    MEMORY_OVERFLOW,
    INVALID_ADDRESS 
} ErrorName;

/* Single error structure */
typedef struct { 
    ErrorName name;                 /* type of error */
    int line;                      /* line number where error occurred */
    char message[MAX_MESSAGE];     /* error message text */
} Error;

/* Error manager - holds all errors found during assembly */
typedef struct {
    Error list[MAX_ERROR];         /* array of errors */
    int count;                     /* number of errors found */
} ErrorManager;

/* Main functions */
void initialize_error_manager(ErrorManager* mgr);
void add_error(ErrorManager* error_mgr, ErrorName name, int line);
int has_errors(ErrorManager* error_mgr);
void print_all_errors(ErrorManager* error_mgr);
void clear_errors(ErrorManager* error_mgr);

/* Validation functions */
int isValid_error_name(ErrorName name);
int isValid_error_line(int line);
int isValid_error_message(const char* message);
int isValid_symbol(struct Symbol* symbol, ErrorManager* error_mgr, int line);
int isValid_error_manager(ErrorManager* error_mgr);
int is_error_mgr_initialized(ErrorManager* error_mgr);

/* Helper functions */
char* get_error_message(ErrorName name);
void failed_alloc_error_mgr(void);

#endif
