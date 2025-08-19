/* This heather contains the structs and functions needed for memory manegment */

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "constants.h"

typedef struct ExternUsage {
    char symbol_name[MAX_SYMBOL_NAME];
    int address;
    struct ExternUsage* next;
} ExternUsage;

typedef struct {
    int IC;
    int DC;
    int code_image[MAX_MEMORY];
    int data_image[MAX_MEMORY];
    ExternUsage* extern_list;
} MemoryManager;

MemoryManager* create_memory_manager(void);
void initialize_memory_manager(MemoryManager* memory);
void add_extern_usage(MemoryManager* memory, const char* symbol_name, int address);
void free_extern_list(ExternUsage* extern_list);
void free_memory_manager(MemoryManager* memory);

#endif
