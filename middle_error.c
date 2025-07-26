#include "middle_error.h"
#include <stdio.h>

void symbol_already_defined_error(const char* name) {
    fprintf(stderr, "Error: Symbol '%s' is already defined.\n", name);
}
