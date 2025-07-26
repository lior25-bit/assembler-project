#include "first_pass.h"
#include <string.h>

int first_pass(ASTNode* ast_head, SymbolTable* symbols, MemoryManager* memory) {
    memory->IC = 0;
    memory->DC = 0;

    ASTNode* current = ast_head;
    while (current != NULL) {
        process_node(current, symbols, memory);
        current = current->next;
    }

    // TODO: update_data_symbols(symbols, memory->IC);

    return 0;
}

int process_node(ASTNode* node, SymbolTable* symbols, MemoryManager* memory) {
    if (node->label != NULL) {
        int is_data = is_directive(node);
        int is_extern = is_extern_directive(node);

        int address;
        if (is_data) {
            address = memory->DC;
        } else {
            address = memory->IC + MEMORY_START;
        }

        add_symbol(node->label, address, is_data, is_extern);
    }

    if (is_directive(node)) {
        memory->DC += node->data_size;
    } else {
        memory->IC += calculate_instruction_size(node);
    }

    return 0;
}

int is_directive(ASTNode* node) {
    switch (node->opcode) {
        case DIR_DATA:
        case DIR_STRING:
        case DIR_MAT:
        case DIR_ENTRY:
        case DIR_EXTERN:
            return 1;
        default:
            return 0;
    }
}

int is_extern_directive(ASTNode* node) {
    return node->opcode == DIR_EXTERN;
}

int calculate_instruction_size(ASTNode* node) {
    // שלב ראשון — נחזיר 1 תמיד
    return 1;
}
