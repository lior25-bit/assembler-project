#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

extern int has_errors;

void reset_errors();
void report_error(const char* message);
void print_all_errors(); 

#endif
