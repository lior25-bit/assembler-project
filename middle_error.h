#ifndef MIDDLE_ERROR_H
#define MIDDLE_ERROR_H

#define MAX_ERRORS 100
extern Error error_list[MAX_ERRORS];
extern int error_count;

/* 
  Chosen size 200 allows storing detailed error messages,
  including label names, line numbers, and formatted descriptions.
  This value balances expressiveness with reasonable memory usage.
*/
typedef struct {
    int line_number;
    char message[200];
} Error;

void log_error(int line_number, const char* format, ...);
void print_all_errors(void);

void symbol_already_defined_error(const char* name);

void invalid_label_error(const char* name);
void label_on_extern_error(const char* label);
void unknown_opcode_error(int opcode);
void invalid_operand_type_error(const char* msg);
void wrong_operand_count_error(int expected, int actual);
void middle_error(const char* format, ...);
void entry_label_error(const char* name);
void extern_and_entry_label_error(const char* name);
void empty_label_error(const char* name);
void no_label_error(const char* name);
void illigal_label(const char* name);
void illigal_start_label_error(const char* name);
void label_too_long_error(const char* name);
void reserved_word_label_error(const char* name);

#endif
