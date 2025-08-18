#include "file_export.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Convert number to base 4 string */
char* convert_to_base_four(int num) {
    static char result[20];
    int i = 0;
    
    if (num == 0) {
        result[0] = '0';
        result[1] = '\0';
        return result;
    }
    
    /* Convert to base 4 */
    while (num > 0) {
        result[i] = (num % 4) + '0';
        num = num / 4;
        i++;
    }
    result[i] = '\0';
    
    /* Reverse string */
    {
        int start = 0;
        int end = i - 1;
        char temp;
        while (start < end) {
            temp = result[start];
            result[start] = result[end];
            result[end] = temp;
            start++;
            end--;
        }
    }
    
    return result;
}

/* Export .ob file */
int export_ob_file(char* file_name, MemoryManager* memory) {
    FILE* file;
    int i;
    
    if (!memory || !file_name) {
        return 0;
    }
    
    file = fopen(file_name, "w");
    if (!file) {
        return 0;
    }
    
    /* Write header */
    fprintf(file, "%d %d\n", memory->IC - MEMORY_START, memory->DC);
    
    /* Write code */
    for (i = MEMORY_START; i < memory->IC; i++) {
        fprintf(file, "%s %s\n", 
                convert_to_base_four(i), 
                convert_to_base_four(memory->code_image[i]));
    }
    
    /* Write data */
    for (i = 0; i < memory->DC; i++) {
        fprintf(file, "%s %s\n", 
                convert_to_base_four(memory->IC + i), 
                convert_to_base_four(memory->data_image[i]));
    }
    
    fclose(file);
    return 1;
}

/* Check if we have extern symbols */
int has_extern_references(MemoryManager* memory) {
    if (!memory) {
        return 0;
    }
    if (memory->extern_list == NULL) {
        return 0;
    }
    return 1;
}

/* Export .ext file */
int export_ext_file(char* file_name, MemoryManager* memory) {
    FILE* file;
    ExternUsage* current;
    
    if (!memory || !file_name) {
        return 0;
    }
    
    if (!has_extern_references(memory)) {
        return 1;
    }
    
    file = fopen(file_name, "w");
    if (!file) {
        return 0;
    }
    
    current = memory->extern_list;
    while (current != NULL) {
        fprintf(file, "%s %s\n", 
                current->symbol_name, 
                convert_to_base_four(current->address));
        current = current->next;
    }
    
    fclose(file);
    return 1;
}

/* Export all files */
int export_all_files(char* file_name, MemoryManager* memory, SymbolTable* table, ASTNode* expanded_ast) {
    char ob_name[100];
    char ext_name[100];
    
    if (!memory || !file_name) {
        return 0;
    }
    
    /* Make file names */
    strcpy(ob_name, file_name);
    strcat(ob_name, ".ob");
    
    strcpy(ext_name, file_name);
    strcat(ext_name, ".ext");
    
    /* Export ob file */
    if (!export_ob_file(ob_name, memory)) {
        return 0;
    }
    
    /* Export ext file if needed */
    if (has_extern_references(memory)) {
        if (!export_ext_file(ext_name, memory)) {
            return 0;
        }
    }
    
    return 1;
}
