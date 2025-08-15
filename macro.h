#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>  
#include <stdlib.h>
#include <string.h>

#define MAX_MACROS 100
#define MAX_MACRO_NAME 31
#define MAX_LINE_LENGTH 82


typedef struct {
    char name[MAX_MACRO_NAME];  
    char** lines;             
    int line_count;             
    int capacity;               
} Macro;

/*the main function of the macro part, taking care of all the important details including open a 
* file creating am file and copy the macro to the new file without the macro callings*/
void run_macro_processor(const char* filename, Macro macro_table[], int* macro_count);

/* the function adds a macro to the macro table */
int add_macro(const char* name, Macro macro_table[], int* macro_count);

/* adding new line  to the macro table */
void add_line_to_macro(Macro* macro, const char* line);

/* searching macro by its name in the table*/
Macro* find_macro(const char* name, Macro macro_table[], int macro_count);

/*writing the line of the macro to the new am file*/
void write_macro_lines(FILE* out, Macro* macro);

/* free the memory of all the macros*/
void free_macros(Macro macro_table[], int macro_count);

/* start, end, call, normal */
const char* what_type_is_macro(const char* line, char* macro_name_out,
                                Macro macro_table[], int macro_count);

/* the function checks if we are at macro start */
int is_macro_start(const char* line, char* macro_name);

/* the function checks if we are at macro end */
int is_macro_end(const char* line);

/* the function checks if we are at macro calling */
int is_macro_call(const char* line, char* macro_name_out,
                  Macro macro_table[], int macro_count);

#endif  /* MACRO_H */
