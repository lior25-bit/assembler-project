#include "middle_error.h"

void symbol_already_defined_error(const char* name) {
    fprintf(stderr, "Error: Symbol '%s' is already defined.\n", name);
}

void entry_label_error(const char* name) {
     fprintf(stderr, "Error: Label '%s' cannot be defined as entry.\n", name);
}

void extern_and_entry_label_error(const char* name) {
      fprintf(stderr, "Error: Label '%s' cannot be defined as both entry and extern.\n", name);
}

void empty_label_error(const char* name) {
      fprintf(stderr, "Error: Label '%s' cannot be empty.\n", name);   
}

void no_label_error(const char* name) {
    fprintf(stderr, "Error: Label '%s' does not exist.\n", name);
}

void illigal_label(const char* name) {
    fprintf(stderr, "Error: Label '%s' contains illegal characters.\n", name);
}

void illigal_start_label_error(const char* name) {
     fprintf(stderr, "Error: Label '%s' does not start with an alphabetic letter.\n", name);
}

void label_too_long_error(const char* name) {
    fprintf(stderr, "Error: Label '%s' exceeds the allowed length. Max length is %d characters.\n", name, MAX_LABEL);
}
