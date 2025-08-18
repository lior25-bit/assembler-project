#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>

/* Create and allocate new memory manager */
MemoryManager* create_memory_manager() {
    MemoryManager* memory = malloc(sizeof(MemoryManager));
    if (!memory) {
        return NULL; 
    }
    
    /* Initialize the memory manager */
    initialize_memory_manager(memory);
    
    return memory;
}

/* Initialize memory manager with default values */
void initialize_memory_manager(MemoryManager* memory) {
    int i; /* C90 variable declaration */
    
    if (!memory) {
        return;
    }
    
    /* Set counters */
    memory->IC = MEMORY_START;  
    memory->DC = 0;
    memory->extern_list = NULL;
    
    /* Initialize code image array to zero */
    for (i = 0; i < MAX_MEMORY; i++) {
        memory->code_image[i] = 0;
    }
    
    /* Initialize data image array to zero */
    for (i = 0; i < MAX_MEMORY; i++) {
        memory->data_image[i] = 0;
    }
}

/* Add extern symbol usage to the list */
void add_extern_usage(MemoryManager* memory, const char* symbol_name, int address) {
    ExternUsage* new_extern;
    
    /* Check if memory manager is valid */
    if (!memory || !symbol_name) {
        return;
    }
    
    /* Allocate memory for new extern usage */
    new_extern = malloc(sizeof(ExternUsage));
    if (!new_extern) {
        return; /* allocation failed */
    }
    
    /* Copy symbol name */
    strcpy(new_extern->symbol_name, symbol_name);
    new_extern->address = address;
    
    /* Add to beginning of linked list */
    new_extern->next = memory->extern_list;
    memory->extern_list = new_extern;
}

/* Free the extern usage linked list */
void free_extern_list(ExternUsage* extern_list) {
    ExternUsage* current = extern_list;
    ExternUsage* next;
    
    /* Go through the list and free each node */
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

/* Free memory manager and all its resources */
void free_memory_manager(MemoryManager* memory) {
    if (!memory) {
        return;
    }
    
    /* Free the extern list first */
    free_extern_list(memory->extern_list);
    
    /* Free the memory manager itself */
    free(memory);
}
