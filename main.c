#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  "macro.h"



int main() {
    Macro macro_table[MAX_MACROS];
    int macro_count = 0;

     run_macro_processor("input2", macro_table, &macro_count);

    return 0;
}


