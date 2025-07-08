#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>  
#include <stdlib.h>
#include <string.h>

#define MAX_MACROS 100
#define MAX_MACRO_NAME 31
#define MAX_LINE_LENGTH 82

/* מבנה המאקרו */
typedef struct {
    char name[MAX_MACRO_NAME];  
    char** lines;             
    int line_count;             
    int capacity;               
} Macro;

/* הפונקציה הראשית של שלב הקדם */
void run_macro_processor(const char* filename, Macro macro_table[], int* macro_count);

/* הוספת מאקרו לטבלה */
int add_macro(const char* name, Macro macro_table[], int* macro_count);

/* הוספת שורה למאקרו */
void add_line_to_macro(Macro* macro, const char* line);

/* חיפוש מאקרו לפי שם */
Macro* find_macro(const char* name, Macro macro_table[], int macro_count);

/* כתיבת השורות של מאקרו לקובץ */
void write_macro_lines(FILE* out, Macro* macro);

/* שחרור כל הזיכרון של כל המאקרואים */
void free_macros(Macro macro_table[], int macro_count);

/* זיהוי סוג השורה (start, end, call, normal) */
const char* what_type_is_macro(const char* line, char* macro_name_out,
                                Macro macro_table[], int macro_count);

/* זיהוי התחלת מאקרו */
int is_macro_start(const char* line, char* macro_name);

/* זיהוי סוף מאקרו */
int is_macro_end(const char* line);

/* זיהוי קריאה למאקרו */
int is_macro_call(const char* line, char* macro_name_out,
                  Macro macro_table[], int macro_count);

#endif  /* MACRO_H */
