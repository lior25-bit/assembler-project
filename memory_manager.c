/* This file contains the memory management functions for the assembler */

#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>

/* Create and allocate new memory manager */
MemoryManager* create_memory_manager(void) {
    MemoryManager* memory = (MemoryManager*)malloc(sizeof(MemoryManager));
    if (!memory) {
        return NULL;
    }
    initialize_memory_manager(memory);
    return memory;
}

/* Initialize memory manager with default values */
void initialize_memory_manager(MemoryManager* memory) {
    int i;
    if (!memory) {
        return;
    }

    memory->IC = MEMORY_START;
    memory->DC = 0;
    memory->extern_list = NULL;

    for (i = 0; i < MAX_MEMORY; i++) {
        memory->code_image[i] = 0;
    }
    for (i = 0; i < MAX_MEMORY; i++) {
        memory->data_image[i] = 0;
    }
}

/* Add extern symbol usage to the list */
void add_extern_usage(MemoryManager* memory, const char* symbol_name, int address) {
    ExternUsage* new_extern;

    if (!memory || !symbol_name || !symbol_name[0]) {
        return;
    }
    if (address < MIN_ADDRESS || address > MAX_ADDRESS) {
        return;
    }
    new_extern = (ExternUsage*)malloc(sizeof(ExternUsage));
    if (!new_extern) {
        return;
    }
    strncpy(new_extern->symbol_name, symbol_name, MAX_SYMBOL_NAME - 1);
    new_extern->symbol_name[MAX_SYMBOL_NAME - 1] = '\0';

    new_extern->address = address;
    new_extern->next = memory->extern_list;
    memory->extern_list = new_extern;
}
/* Free the extern usage linked list */
void free_extern_list(ExternUsage* extern_list) {
    ExternUsage* current = extern_list;
    ExternUsage* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}
/* Free memory manager */
void free_memory_manager(MemoryManager* memory) {
    if (!memory) {
        return;
    }
    free_extern_list(memory->extern_list);
    memory->extern_list = NULL;
    free(memory);
}
