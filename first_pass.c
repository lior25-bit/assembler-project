/* This file contains the functions of the first pass of the assembler */

#include "first_pass.h"
#include "ast.h"
#include "ast_logic.h"
#include <string.h>

/* This function is the main function of the first pass */
int first_pass(ASTNode* ast_head, MemoryManager* memory, SymbolTable* symbol_table, ErrorManager* error_mgr) {
    ASTNode* current;

    memory->IC = MEMORY_START;
    memory->DC = 0;

    current = ast_head;
    while (current != NULL) {
        process_node(current, memory, symbol_table, error_mgr);
        current = current->next;
    }
    update_data_symbols(symbol_table, memory->IC);
    print_all_errors(error_mgr);

    return 1;
}

/* This function is processing a given node */
int process_node(ASTNode* node, MemoryManager* memory, SymbolTable* symbol_table, ErrorManager* error_mgr) {
    if(!validate_full_node_params(node, memory, symbol_table, error_mgr)) {
        return 0;
    }

    if(!handle_node_label(node, memory, symbol_table, error_mgr)) {
        return 0;
    }
    
    if(is_directive_node(node)) {
        if(!process_directive_node(node, memory, error_mgr)){
            return 0;
        }
        return 1;
    }

    if(is_instruction_node(node)) {
        if(!process_instruction_node(node, memory, error_mgr)) {
            return 0;
        }
        return 1;
    }
    return 0;
}

/* This function validates the basic parameters of a node */
int validate_basic_node_params(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    if(!node || !memory) {
        add_error(error_mgr, ALLOC_FAILED, 0);
        return 0;
    }
    if(!error_mgr) {
        failed_alloc_error_mgr();
        return 0;
    }
    return 1;
}

/* This function validates the extended paramenters of a node */
int validate_full_node_params(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    if(!node || !memory || !table) {
        add_error(error_mgr, ALLOC_FAILED, 0);
        return 0;
    }
    if(!error_mgr) {
        failed_alloc_error_mgr();
        return 0;
    }
    return 1;
}

/* This function processes an instruction node */
int process_instruction_node(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    int size;

    if(!validate_basic_node_params(node, memory, error_mgr)) {
        return 0;
    }

    if(memory->IC < MEMORY_START || memory->IC > MAX_ADDRESS) {
        add_error(error_mgr, INVALID_ADDRESS, node->original_line);
    }

    if(!is_valid_addressing_mode(node->opcode,
                                 node->operands[0].type,
                                 node->operands[1].type,
                                 error_mgr,
                                 node->original_line)) {
        add_error(error_mgr, INVALID_ADDRESSING_MODE, node->original_line);
    }

    size = calculate_instruction_size(node, error_mgr);
    memory->IC += size;

    return 1;
}

/* This function process a directive node */
int process_directive_node(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    if(!validate_basic_node_params(node, memory, error_mgr)) {
        return 0;
    }
    
    switch (node->directive.dir_type) { 
        case DIR_DATA: 
            return handle_data_directive(node, memory, error_mgr); 
        case DIR_STRING: 
            return handle_string_directive(node, memory, error_mgr); 
        case DIR_MAT: 
            return handle_mat_directive(node, memory, error_mgr); 
        case DIR_EXTERN: 
            return 1;
        case DIR_ENTRY: 
            return 1;  
        default: 
            add_error(error_mgr, UNKNOWN_OPCODE, node->original_line); 
            return 0; 
    }
}

/* This function handle dara directuve */
int handle_data_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    int directive_size;

    if(!validate_basic_node_params(node, memory, error_mgr)) {
        return 0;
    }
    
    if(node->directive.value.data.count <= 0) {
        add_error(error_mgr, INVALID_OPCODE, node->original_line);
        return 0;
    }
    
    directive_size = node->directive.value.data.count;
    
    if(memory->DC + directive_size > MAX_ADDRESS) {
        add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
        return 0;
    }
    
    memory->DC += directive_size;
    return 1;
}

/* This function process the node type */
SymbolType process_node_type(ASTNode* node) {
    if(!node) {
        return SYMBOL_NONE;
    }
    if (is_extern_directive_node(node)) {
        return SYMBOL_EXTERN;
    } 
    else if (is_directive_node(node)) {
        return SYMBOL_DATA;
    } 
    else {
        return SYMBOL_CODE; 
    }
}

/* This function handle the node's label */
int handle_node_label(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    SymbolType type;
    int address;
    Symbol* symbol;

    if(!node->label) {
        return 1;
    }
    
    if(is_entry_directive_node(node)) {
        add_error(error_mgr, ENTRY_LABEL, node->original_line);
        return 0;
    }
    
    type = process_node_type(node);
    if(type == SYMBOL_DATA) {
        address = memory->DC;
    }
    else {
        address = memory->IC;
    }

    if(find_symbol(table, node->label)) {
        add_error(error_mgr, SYMBOL_ALREADY_DEFINED, node->original_line);
        return 0;
    }
    
    symbol = create_symbol(node->label, address, type);
    if(!symbol) {
        add_error(error_mgr, ALLOC_FAILED, node->original_line);
        return 0;
    }
    
    if(!add_last_symbol(table, symbol)) {
        add_error(error_mgr, ALLOC_FAILED, node->original_line);
        return 0;
    }
    
    return 1;
}

/* This function checks if the addressing mode is valid */
int is_valid_addressing_mode(OpCode op, ArgType src, ArgType dest, ErrorManager* error_mgr, const char* og_line) {
    if(op == OP_NONE) {
        add_error(error_mgr, INVALID_ADDRESSING_MODE, og_line);
        return 0;
    }

    if(is_two_op_instruction(op) && op != OP_LEA) {
        if(src >= ARG_IMMEDIATE && src <= ARG_MATRIX) {
            if(dest >= ARG_DIRECT && dest <= ARG_REGISTER) {
                return 1;
            }
        }
        return 0;
    }
    if(is_one_op_instruction(op) && op != OP_PRN) { 
        if(dest >= ARG_DIRECT && dest <= ARG_REGISTER) {
            return 1;
        }
        return 0;    
    }
    if(is_zero_op_instruction(op)) {
        if((src != ARG_NONE) || (dest != ARG_NONE) ) {
            return 0;
        }
        return 1;
    }
    return 0;
}

/* This function handle string directive nodes */
int handle_string_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    int string_length;

    if(!validate_basic_node_params(node, memory, error_mgr)) {
        return 0;
    }
    
    if(!node->directive.value.string_val) {
        add_error(error_mgr, INVALID_OPCODE, node->original_line);
        return 0;
    }
    
    string_length = (int)strlen(node->directive.value.string_val) + 1;
    
    if(memory->DC + string_length > MAX_ADDRESS) {
        add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
        return 0;
    }
    
    memory->DC += string_length;
    return 1;
}

/* This function handle matrix directive nodes */
int handle_mat_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    int matrix_size;

    if(!validate_basic_node_params(node, memory, error_mgr)) {
        return 0;
    }
    
    if(node->directive.value.mat.rows <= 0 || node->directive.value.mat.cols <= 0) {
        add_error(error_mgr, INVALID_OPCODE, node->original_line);
        return 0;
    }
    
    matrix_size = calculate_matrix_memory(node->directive.value.mat.rows, node->directive.value.mat.cols);
    
    if(memory->DC + matrix_size > MAX_ADDRESS) {
        add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
        return 0;
    }
    
    memory->DC += matrix_size;
    return 1;
}

/* This function calculates the memory required for a matrix */
int calculate_matrix_memory(int rows, int cols) {
    return (rows * cols);
}

/* This function calculates the size of an instruction */
int calculate_instruction_size(ASTNode* node, ErrorManager* error_mgr) {
    int size;
    Operand src;
    Operand dest;

    if(!error_mgr) {
        failed_alloc_error_mgr();
        return 0;
    }
    if(!node) {
        add_error(error_mgr, ALLOC_FAILED, 0);
        return 0;
    }

    size = 1;
    src = node->operands[0];
    dest = node->operands[1];

    switch (node->opcode) {
        case OP_MOV:
        case OP_CMP:
        case OP_ADD:
        case OP_SUB:
        case OP_LEA:
            if (src.type == ARG_REGISTER && dest.type == ARG_REGISTER) {
                size += 1;
            }
            else {
                if (src.type == ARG_MATRIX) size += 3;
                else if (src.type != ARG_NONE) size += 1;

                if (dest.type == ARG_MATRIX) size += 3;
                else if (dest.type != ARG_NONE) size += 1;
            }
            break;

        case OP_CLR:
        case OP_NOT:
        case OP_INC:
        case OP_DEC:
        case OP_JMP:
        case OP_BNE:
        case OP_JSR:
        case OP_RED:
        case OP_PRN:
            if (dest.type == ARG_MATRIX) size += 3;
            else if (dest.type != ARG_NONE) size += 1;
        break;

        case OP_RTS:
        case OP_STOP:
        break;

        default:
            add_error(error_mgr, INVALID_OPCODE, node->original_line);
            return 0;
    }

    return size;
}
