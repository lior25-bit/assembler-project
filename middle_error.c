#include "middle_error.h"

void symbol_already_defined_error(const char* name) {
    fprintf(stderr, "Error: Symbol '%s' is already defined.\n", name);
}

void entry_on_external_error(const char* name) {
    fprintf(stderr, "Error: Symbol '%s' cannot be defined as both entry and extern.\n", name);
}
