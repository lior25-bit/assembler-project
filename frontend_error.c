
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frontend_error.h"


/**
 * This function uses frontend header and prints the error message and what line got the problem. Parser.c calls this function if there is a problem 
 * @param err - a pointer to FrontErrorInfo that contains the every frontend errors info.
 */
void front_error_report(const FrontErrorInfo *err) {
    if (!err) {
        fprintf(stderr, "Unknown error\n");
        return;
    }
    fprintf(stderr,
        "ERROR CODE: %d | LINE: %d\nERROR: %s\n",
        err->code, err->line, err->message);
}
