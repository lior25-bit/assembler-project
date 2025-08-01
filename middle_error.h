#ifndef MIDDLE_ERROR_H
#define MIDDLE_ERROR_H

void symbol_already_defined_error(const char* name);
void invalid_label_error(const char* name);
void label_on_extern_error(const char* label);
void unknown_opcode_error(int opcode);
void invalid_operand_type_error(const char* msg);
void wrong_operand_count_error(int expected, int actual);
void middle_error(const char* format, ...);

#endif /* MIDDLE_ERROR_ H*/
