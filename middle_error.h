#ifndef MIDDLE_ERROR_H
#define MIDDLE_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*report when a symbol is already defiend*/
void symbol_already_defined_error(const char* name);
void entry_on_external_error(const char* name);

#endif /* MIDDLE_ERROR_H */
