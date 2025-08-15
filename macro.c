#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"  

/*This function will state in which part of the macro we are */
const char* what_type_is_macro(const char* line, char* macro_name_out, Macro macro_table[], int macro_count) {
    if (is_macro_start(line, macro_name_out))
        return "macro_start";

    if (is_macro_end(line))
        return "macro_end";

    if (is_macro_call(line, macro_name_out, macro_table, macro_count))
        return "macro_call";

    return "normal";
}
/*the function checks if we are t the start of the macro
 * True if the line begins with "mcro <name>".
 * We grab the name with sscanf and copy it safely.*/
int is_macro_start(const char* line, char* macro_name) {
    char word[MAX_LINE_LENGTH];
    if (strncmp(line, "mcro ", 5) == 0 && sscanf(line, "mcro %s", word) == 1) {
        strncpy(macro_name, word, MAX_MACRO_NAME);
        macro_name[MAX_MACRO_NAME - 1] = '\0';
        return 1;
    }
    return 0;
    }
/*the function checks if we are t the end of the macro
* is_macro_end
* True if the first token is exactly "mcroend".
* We tokenize a stack copy so we don't touch the original line.*/
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
/* A call is when the first word equals the name of a macro we
 * already collected.*/
int is_macro_call(const char* line, char* macro_name_out,Macro macro_table[], int macro_count) {
    char word[MAX_LINE_LENGTH];
    int i;
    if (sscanf(line, "%s", word) == 1) {
        for (i = 0; i < macro_count; i++) {
            if (strcmp(word, macro_table[i].name) == 0) {
                strncpy(macro_name_out, word, MAX_MACRO_NAME-1);
                macro_name_out[MAX_MACRO_NAME - 1] = '\0';
                return 1;
            }
        }
    }
    return 0;
}

/*open <filename>.as (input), <filename>.am (output)
 * and scan lines in order to 
 *collect mode between mcro/mcroend
 *expand call by writing saved lines
 * otherwise copy line as-is to .am
 *free all macro storage before returning*/
void run_macro_processor(const char* filename, Macro macro_table[], int* macro_count) {
    char line_buffer[MAX_LINE_LENGTH];
    char macro_name[MAX_MACRO_NAME];
    const char* type;
    int in_macro = 0;
    Macro* current_macro = NULL;
    Macro* m ;
    char as_file_name[FILENAME_MAX];
    char am_file_name[FILENAME_MAX];

    FILE* as_file;
    FILE* am_file;


     /* Build "<filename>.as" and "<filename>.am" */
    strcpy(as_file_name, filename);        
    strcat(as_file_name, ".as");            

    strcpy(am_file_name, filename);         
    strcat(am_file_name, ".am");            

      /* Open input .as */
    as_file = fopen(as_file_name, "r");
    if (!as_file) {
        fprintf(stderr, "A problem with opening source file  %s\n", as_file_name);
        return;
    }

    /* Open output .am */
    am_file = fopen(am_file_name, "w");
    if (!am_file) {
        fprintf(stderr, "A problem with creating input file %s\n", am_file_name);
        fclose(as_file);
        return;
    }

      /* Line-by-line pass */
  while (fgets(line_buffer, MAX_LINE_LENGTH, as_file)) {
        type = what_type_is_macro(line_buffer, macro_name, macro_table, *macro_count);
          /* We are recording macro body lines until mcroend */
        if (in_macro && current_macro) {
            if (strcmp(type, "macro_end") == 0) {
                in_macro = 0;
                current_macro = NULL;
            } else {
                add_line_to_macro(current_macro, line_buffer);
            }
        } else if (strcmp(type, "macro_start") == 0) {  /* Start a new macro slot */
            if (!in_macro) {
                in_macro = 1;
                if (add_macro(macro_name, macro_table, macro_count)) {
                    current_macro = &macro_table[*macro_count - 1];
                }
            }
        } else if (strcmp(type, "macro_call") == 0) {
               /* Expand saved lines */
            m = find_macro(macro_name, macro_table, *macro_count);
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
/* add_macro
 * Adds a new Macro record with initial capacity = 10.
 * Returns 1 on success, 0 on failure (limit/alloc).*/
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
/* add_line_to_macro
 * Append a COPY of 'line' to macro->lines; grow capacity x2 if needed.*/
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

/*search macro by its name return a pointer to the makro table */
Macro* find_macro(const char* name, Macro macro_table[], int macro_count) {
    int i;
    for (i = 0; i < macro_count; i++) {
        if (strcmp(macro_table[i].name, name) == 0) {
            return &macro_table[i];
        }
    }
    return NULL;
}
/* write_macro_lines
 * Emit stored lines of a macro into 'out' in order.*/
void write_macro_lines(FILE* out, Macro* macro) {
    int i;
    for (i = 0; i < macro->line_count; i++) {
        fputs(macro->lines[i], out);
    }
} 
/* free_macros
 * Free every allocated line, then free each macro->lines array.*/
void free_macros(Macro macro_table[], int macro_count) {
    int i, j;
    for (i = 0; i < macro_count; i++) {
        for (j = 0; j < macro_table[i].line_count; j++) {
            free(macro_table[i].lines[j]);
        }
        free(macro_table[i].lines);
    }
}



