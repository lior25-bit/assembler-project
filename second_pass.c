#include "second_pass.h"
#include "symbol_table.h"
#include <stdio.h>
#include "ast.h"
#include "first_pass.h"
#include "Second_pass_errors.h"
#include "constants.h"


int second_pass(ASTNode* ast_head, MemoryManager* memory) {
    memory->IC = 0;
    memory->DC = 0;

    ASTNode* current = ast_head;
    while(current != NULL) {
        process_second_pass(current, memory);
        current = current->next;
    }

    return 0;
}

int process_node_second_pass(ASTNode* node, MemoryManager* memory) {
if(is_directive(node)) {
    if(node->opcode == DIR_ENTRY) { /* only updates the Symbol*/
        handle_entry(node);
    }
    else {
        process_directive(node,memory);
    }
}

else {
    process_instruction(node,memory);
 }
 return 0;
} 


void handle_entry(ASTNode* node) {
    char* entry_label = node->operands[0].label; /* label is enough since we handle entry */ 
    Symbol* sym = find_symbol(entry_label);

    if (sym == NULL) {
        print_error_entry(entry_label);
        return;
    }

    if (sym->is_extern) {
        print_error_extern(entry_label);
        return;
    }
    /* sym is entry */
    sym->is_entry = 1;
    return;
}

void process_directive(ASTNode* node, MemoryManager* memory) {
    int i;

    switch (node->opcode) {
        case DIR_DATA:
            for (i = 0; i < node->data_count; i++) {
                memory->data_image[memory->DC] = node->data_values[i];
                memory->DC++;
            }
            break;

        case DIR_STRING:
            for (i = 0; node->string_value[i] != '\0'; i++) {
                memory->data_image[memory->DC] = (int)node->string_value[i];
                memory->DC++;
            }
            
            memory->data_image[memory->DC] = 0;
            memory->DC++;
            break;

        case DIR_MAT:
            for (i = 0; i < node->data_count; i++) {
                memory->data_image[memory->DC] = node->data_values[i];
                memory->DC++;
            }
            break;

        default:
            break;
    }
}

/* This function coding the instructions and insert them to the memory */
void process_instruction(ASTNode* node, MemoryManager* memory) {
    int current_address = memory->IC + MEMORY_START;
    Operand src_op = node->operands[0];
    Operand dest_op = node->operands[1]; 

    switch (src_op.type)
    {
    case ARG_DIRECT: {
         Symbol* sym = find_symbol(src_op.label);
        if(sym == NULL) {
            symbol_does_not_exist(sym);
            return;
        }
        add__direct_code_to_memory(sym,memory);
        break;
    }
    case ARG_MATRIX: {
        Symbol* sym = find_symbol(src_op.label);
        if(sym == NULL) {
            symbol_does_not_exist(sym);
            return;
        }
        add_mat_code_to_memory(sym,memory);
        break;
    }
    case ARG_IMMEDIATE: {
        int value = src_op.value;
        if(value < -512 || value > 511) { /*Immidiate value range */
            /* eror message*/
            return;
        }
        add_immidate_code_to_memory(value,memory);
        break;
    }
    case ARG_REGISTER: {
        int value = src_op.reg_value;
        if(value < 0 || value > 7) { /* register value range */
            /*error*/
            return;
        }
        add_register_code_to_memory(value,memory);
        break;
    }
    case ARG_NONE: {
        break;
    }

    default:
        /*eror unknown operand*/
        return;
}

void add_direct_code_to_memory(Symbol* sym, MemoryManager* memory) {
    int address = sym->address;
    int word = address << ARE_SHIFT; /* shift the value of two bits to make space for ARE values */

    if(sym->type == SYMBOL_EXTERN) {
        /*e*/
    }
    else {
        /*r*/
    }
    memory->code_image[memory->IC++] =  word; /* increasing IC after being added to memory code */
}

void add_mat_code_to_memory(Symbol* sym, MemoryManager* memory) {
    int adress = sym->address;
    int word = adress << ARE_SHIFT;
    /*A times 2*/
}

void add_immidate_code_to_memory(int value, MemoryManager* memory) {
    /*A*/
}

void  add_register_code_to_memory(int value, MemoryManager* memory) {
    /*A*/
}
