/* This file manages error reporting for the assembler */
#include "error_manager.h"
#include <string.h>
#include <stdio.h>


/* This function initializes the ErrorManager struct */
void initialize_error_manager(ErrorManager* mgr) {
    if (!mgr) {
        return;
    }
    mgr->count = 0;
    clear_errors(mgr);
}

/* This function clear all the errors in a given ErrorManager */
void clear_errors(ErrorManager* mgr) {
    if (!mgr) {
        return;
    }
    memset(mgr->list, 0, sizeof(mgr->list)); /* This functions initilize the entire list */
}

/* This function checks if there are any errors in the ErrorManager struct */
int has_errors(ErrorManager* mgr) {
    if(!mgr) {
        return 0;
    }
    return (mgr->count > 0);
}

/* This function print a single Error*/
static void print_error(Error* error) {
    const char* msg;
    if (!error)  {
        return;
    }

    if (error->message[0] != '\0') {
        msg = error->message;     
    } 
    else {
        msg = get_error_message(error->name);
    }
    printf("%s\n", msg);

    if (error->original_line[0] != '\0') {
        printf("-> %s\n", error->original_line);
    }
}

/* This function prints all errors that are in a given ErrorManager*/
void print_all_errors(ErrorManager* mgr) {
    int i;
    if(!mgr) {
        return;
    }
    for(i = 0; i < mgr->count; i++) {
        print_error(&mgr->list[i]);
    }
}

/* This function adds an error to a given ErrorManager */
void add_error(ErrorManager* mgr, ErrorName name, const char* og_line) {
    const char* msg;
    if (!mgr) return;
    if (mgr->count < 0 || mgr->count >= MAX_ERROR) return;

    mgr->list[mgr->count].name = name;

    msg = get_error_message(name);
    strncpy(mgr->list[mgr->count].message, msg, MAX_MESSAGE - 1);
    mgr->list[mgr->count].message[MAX_MESSAGE - 1] = '\0';

    mgr->list[mgr->count].original_line[0] = '\0';
    if (og_line && og_line[0]) {
        strncpy(mgr->list[mgr->count].original_line, og_line, MAX_LINE - 1);
        mgr->list[mgr->count].original_line[MAX_LINE - 1] = '\0';
    }

    mgr->count++;
}

/* This function returns a string representation of the error message */
const char* get_error_message(ErrorName name) {
    switch (name) {
        case ALLOC_FAILED:            return "Memory allocation failed";
        case TABLE_NOT_ALLOC:         return "Table not allocated";
        case SYMBOL_ALREADY_DEFINED:  return "Symbol already defined";
        case SYMBOL_NO_EXIST:         return "Symbol does not exist";
        case INVALID_SYMBOL_NAME:     return "Invalid symbol name";
        case RESERVED_WORD_SYMBOL:    return "Reserved word used as symbol";
        case INVALID_OPCODE:          return "Invalid opcode";
        case UNKNOWN_OPCODE:          return "Unknown opcode";
        case EXTERN_LABEL:            return "Extern label misuse";
        case ENTRY_LABEL:             return "Entry label misuse";
        case EXTERN_AND_ENTRY_LABEL:  return "Label is both extern and entry";
        case INVALID_ADDRESSING_MODE: return "Invalid addressing mode";
        case MEMORY_OVERFLOW:         return "Memory overflow";
        case INVALID_ADDRESS:         return "Invalid address";
        default:                      return "Unknown error";
    }
}
