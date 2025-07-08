
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"  


const char* what_type_is_macro(const char* line, char* macro_name_out, Macro macro_table[], int macro_count) {
    if (is_macro_start(line, macro_name_out))
        return "macro_start";

    if (is_macro_end(line))
        return "macro_end";

    if (is_macro_call(line, macro_name_out, macro_table, macro_count))
        return "macro_call";

    return "normal";
}

int is_macro_start(const char* line, char* macro_name) {
    char word[MAX_LINE_LENGTH];
    if (strncmp(line, "mcro ", 5) == 0 && sscanf(line, "mcro %s", word) == 1) {
        strncpy(macro_name, word, MAX_MACRO_NAME);
        return 1;
    }
    return 0;
    }

int is_macro_end(const char* line) {
    char copy[MAX_LINE_LENGTH];
    char* token;
    strncpy(copy, line, MAX_LINE_LENGTH);
    copy[MAX_LINE_LENGTH - 1] = '\0';
    token = strtok(copy, " \t\r\n");
    if (token && strcmp(token, "mcroend") == 0) {
        return 1;
    }
    return 0;
}

int is_macro_call(const char* line, char* macro_name_out,Macro macro_table[], int macro_count) {
    char word[MAX_LINE_LENGTH];
    int i;
    if (sscanf(line, "%s", word) == 1) {
        for (i = 0; i < macro_count; i++) {
            if (strcmp(word, macro_table[i].name) == 0) {
                strncpy(macro_name_out, word, MAX_MACRO_NAME);
                return 1;
            }
        }
    }
    return 0;
}


void run_macro_processor(const char* filename, Macro macro_table[], int* macro_count) {
    char line_buffer[MAX_LINE_LENGTH];
    char macro_name[MAX_MACRO_NAME];
    const char* type;
    int in_macro = 0;
    Macro* current_macro = NULL;

    char as_file_name[FILENAME_MAX];
    char am_file_name[FILENAME_MAX];

    FILE* as_file;
    FILE* am_file;

    /* יצירת שמות קבצים */
    strcpy(as_file_name, filename);        
    strcat(as_file_name, ".as");            

    strcpy(am_file_name, filename);         
    strcat(am_file_name, ".am");            

    /* פתיחת קובץ .as לקריאה */
    as_file = fopen(as_file_name, "r");
    if (!as_file) {
        fprintf(stderr, "A problem with opening source file  %s\n", as_file_name);
        return;
    }

    /* פתיחת קובץ .am לכתיבה */
    am_file = fopen(am_file_name, "w");
    if (!am_file) {
        fprintf(stderr, "A problem with creating input file %s\n", am_file_name);
        fclose(as_file);
        return;
    }

    /* קריאת קובץ שורה שורה */
  while (fgets(line_buffer, MAX_LINE_LENGTH, as_file)) {
        type = what_type_is_macro(line_buffer, macro_name, macro_table, *macro_count);

        if (in_macro && current_macro) {
            if (strcmp(type, "macro_end") == 0) {
                in_macro = 0;
                current_macro = NULL;
            } else {
                add_line_to_macro(current_macro, line_buffer);
            }
        } else if (strcmp(type, "macro_start") == 0) {
            if (!in_macro) {
                in_macro = 1;
                if (add_macro(macro_name, macro_table, macro_count)) {
                    current_macro = &macro_table[*macro_count - 1];
                }
            }
        } else if (strcmp(type, "macro_call") == 0) {
            Macro* m = find_macro(macro_name, macro_table, *macro_count);
            if (m) {
                write_macro_lines(am_file, m);
            }
        } else {
            fputs(line_buffer, am_file);
        }
    }

    fclose(as_file);
    fclose(am_file);
    free_macros(macro_table, *macro_count);
}

int add_macro(const char* name, Macro macro_table[], int* macro_count) {
   Macro* m;
    if (*macro_count >= MAX_MACROS) {
        return 0;
    }
    m = &macro_table[*macro_count];
    strncpy(m->name, name, MAX_MACRO_NAME);
    m->name[MAX_MACRO_NAME - 1] = '\0';
    m->capacity = 10;
    m->lines = malloc(sizeof(char*) * m->capacity);
    if (!m->lines) {
        return 0;
    }
    m->line_count = 0;
    (*macro_count)++;
    return 1;
}

void add_line_to_macro(Macro* macro, const char* line) {
    char* copy;
    if (macro->line_count >= macro->capacity) {
        int new_capacity = macro->capacity * 2;
        char** new_line = realloc(macro->lines, new_capacity * sizeof(char*));
        if (!new_line) {
            return;
        }
        macro->lines = new_line;
        macro->capacity = new_capacity;
    }
    copy = malloc(strlen(line) + 1);
    if (!copy) {
        return;
    }
    strcpy(copy, line);
    macro->lines[macro->line_count] = copy;
    macro->line_count++;
}


Macro* find_macro(const char* name, Macro macro_table[], int macro_count) {
    int i;
    for (i = 0; i < macro_count; i++) {
        if (strcmp(macro_table[i].name, name) == 0) {
            return &macro_table[i];
        }
    }
    return NULL;
}

void write_macro_lines(FILE* out, Macro* macro) {
    int i;
    for (i = 0; i < macro->line_count; i++) {
        fputs(macro->lines[i], out);
    }
}

void free_macros(Macro macro_table[], int macro_count) {
    int i, j;
    for (i = 0; i < macro_count; i++) {
        for (j = 0; j < macro_table[i].line_count; j++) {
            free(macro_table[i].lines[j]);
        }
        free(macro_table[i].lines);
    }
}



