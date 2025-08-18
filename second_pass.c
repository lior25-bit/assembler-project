#include "second_pass.h"
#include "ast_logic.h"
#include <stdio.h>
#include <string.h>

int second_pass(ASTNode* ast_head, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    ASTNode* current = ast_head;
    
    while (current != NULL) {
        if (!process_node_second_pass(current, memory, table, error_mgr)) {
            return 0;
        }
        current = current->next;
    }
    
    if (has_errors(error_mgr)) {
        return 0;
    }
    return 1;
}

int process_node_second_pass(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    if (!node) {
        return 0;
    }
    
    if (is_directive_node(node)) {
        return handle_directive_second_pass(node, memory, table, error_mgr);
    } 
    
    if (is_instruction_node(node)) {
        return process_instruction_second_pass(node, memory, table, error_mgr);
    }
    
    return 1;
}

int handle_directive_second_pass(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    if (node->directive.dir_type == DIR_ENTRY) {
        return handle_entry_directive(node, table, error_mgr);
    }
    if (node->directive.dir_type == DIR_EXTERN) {
        return 1;
    }
    return 1;
}

int handle_entry_directive(ASTNode* node, SymbolTable* table, ErrorManager* error_mgr) {
    char* label_name = node->directive.value.ext_label;
    Symbol* symbol = find_symbol(table, label_name);
    
    if (!symbol) {
        add_error(error_mgr, SYMBOL_NO_EXIST, node->line_number);
        return 0;
    }
    
    if (symbol->type == SYMBOL_EXTERN) {
        add_error(error_mgr, EXTERN_AND_ENTRY_LABEL, node->line_number);
        return 0;
    }
    
    mark_entry(table, label_name);
    return 1;
}

int process_instruction_second_pass(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    int result = encode_instruction_word(node, memory, table, error_mgr);
    if (!result) {
        add_error(error_mgr, INVALID_OPCODE, node->line_number);
        return 0;
    }
    return 1;
}

int encode_instruction_word(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    int src_mode = get_addressing_mode_code(node->operands[0].type);
    int dest_mode = get_addressing_mode_code(node->operands[1].type);
    int first_word = build_first_word(node->opcode, src_mode, dest_mode);
    
    write_word_to_memory(memory, first_word);
    
    if (both_operands_are_registers(node)) {
        encode_register_pair(node, memory);
    } else {
        if (!encode_operands_separately(node, memory, table, error_mgr)) {
            return 0;
        }
    }
    
    return 1;
}

int both_operands_are_registers(ASTNode* node) {
    if (node->operands[0].type == ARG_REGISTER && node->operands[1].type == ARG_REGISTER) {
        return 1;
    }
    return 0;
}

void encode_register_pair(ASTNode* node, MemoryManager* memory) {
    int src_reg = node->operands[0].val.reg_value;
    int dest_reg = node->operands[1].val.reg_value;
    int reg_word = build_register_word(src_reg, dest_reg);
    write_word_to_memory(memory, reg_word);
}

int encode_operands_separately(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    if (node->operands[0].type != ARG_NONE) {
        if (!encode_single_operand(node->operands[0], memory, table, 1, error_mgr)) {
            return 0;
        }
    }
    
    if (node->operands[1].type != ARG_NONE) {
        if (!encode_single_operand(node->operands[1], memory, table, 0, error_mgr)) {
            return 0;
        }
    }
    
    return 1;
}

int encode_single_operand(Operand operand, MemoryManager* memory, SymbolTable* table, int is_source, ErrorManager* error_mgr) {
    if (operand.type == ARG_IMMEDIATE) {
        return encode_immediate_operand(operand, memory);
    }
    if (operand.type == ARG_DIRECT) {
        return encode_direct_operand(operand, memory, table, error_mgr);
    }
    if (operand.type == ARG_REGISTER) {
        return encode_register_operand(operand, memory, is_source);
    }
    if (operand.type == ARG_MATRIX) {
        return encode_matrix_operand(operand, memory, table, error_mgr);
    }
    return 0;
}

int encode_immediate_operand(Operand operand, MemoryManager* memory) {
    int value = operand.val.value;
    int word = build_immediate_word(value);
    write_word_to_memory(memory, word);
    return 1;
}

int encode_direct_operand(Operand operand, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    char* label = operand.val.label;
    Symbol* symbol = find_symbol(table, label);
    int word;
    
    if (!symbol) {
        add_error(error_mgr, SYMBOL_NO_EXIST, 0);
        return 0;
    }
    
    if (symbol->type == SYMBOL_EXTERN) {
        word = build_external_word();
        add_external_reference(memory, label, memory->IC);
    } else {
        word = build_relocatable_word(symbol->address);
    }
    
    write_word_to_memory(memory, word);
    return 1;
}

int encode_register_operand(Operand operand, MemoryManager* memory, int is_source) {
    int reg_num = operand.val.reg_value;
    int word;
    
    if (is_source) {
        word = build_source_register_word(reg_num);
    } else {
        word = build_dest_register_word(reg_num);
    }
    
    write_word_to_memory(memory, word);
    return 1;
}

int encode_matrix_operand(Operand operand, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    char* mat_label = operand.mat_label;
    Symbol* symbol = find_symbol(table, mat_label);
    int address_word;
    int row_reg;
    int col_reg;
    int indices_word;
    
    if (!symbol) {
        add_error(error_mgr, SYMBOL_NO_EXIST, 0);
        return 0;
    }
    
    if (symbol->type == SYMBOL_EXTERN) {
        address_word = build_external_word();
        add_external_reference(memory, mat_label, memory->IC);
    } else {
        address_word = build_relocatable_word(symbol->address);
    }
    
    write_word_to_memory(memory, address_word);
    
    row_reg = operand.val.matrix_regs[0];
    col_reg = operand.val.matrix_regs[1];
    indices_word = build_matrix_indices_word(row_reg, col_reg);
    write_word_to_memory(memory, indices_word);
    
    return 2;
}

int build_first_word(OpCode opcode, int src_mode, int dest_mode) {
    int word = 0;
    word = word | ((opcode & OPCODE_MASK) << OPCODE_SHIFT);
    word = word | ((src_mode & ADDRESSING_MODE_MASK) << SRC_MODE_SHIFT);
    word = word | ((dest_mode & ADDRESSING_MODE_MASK) << DEST_MODE_SHIFT);
    word = word | ARE_ABSOLUTE;
    return word;
}

int build_immediate_word(int value) {
    int word = 0;
    word = word | (value & VALUE_MASK);
    word = word | ARE_ABSOLUTE;
    return word;
}

int build_relocatable_word(int address) {
    int word = 0;
    word = word | (address & VALUE_MASK);
    word = word | ARE_RELOCATABLE;
    return word;
}

int build_external_word(void) {
    return ARE_EXTERNAL;
}

int build_register_word(int src_reg, int dest_reg) {
    int word = 0;
    word = word | ((src_reg & REGISTER_MASK) << SRC_REG_SHIFT);
    word = word | ((dest_reg & REGISTER_MASK) << DEST_REG_SHIFT);
    word = word | ARE_ABSOLUTE;
    return word;
}

int build_source_register_word(int reg_num) {
    int word = 0;
    word = word | ((reg_num & REGISTER_MASK) << SRC_REG_SHIFT);
    word = word | ARE_ABSOLUTE;
    return word;
}

int build_dest_register_word(int reg_num) {
    int word = 0;
    word = word | ((reg_num & REGISTER_MASK) << DEST_REG_SHIFT);
    word = word | ARE_ABSOLUTE;
    return word;
}

int build_matrix_indices_word(int row_reg, int col_reg) {
    int word = 0;
    word = word | ((row_reg & REGISTER_MASK) << SRC_REG_SHIFT);
    word = word | ((col_reg & REGISTER_MASK) << DEST_REG_SHIFT);
    word = word | ARE_ABSOLUTE;
    return word;
}

void write_word_to_memory(MemoryManager* memory, int word) {
    int index = memory->IC - MEMORY_START;
    memory->code_image[index] = word;
    memory->IC = memory->IC + 1;
}

void add_external_reference(MemoryManager* memory, char* symbol_name, int address) {
    if (memory->external_count < MAX_EXTERNALS) {
        strcpy(memory->external_refs[memory->external_count].symbol_name, symbol_name);
        memory->external_refs[memory->external_count].address = address;
        memory->external_count = memory->external_count + 1;
    }
}

int get_addressing_mode_code(ArgType type) {
    if (type == ARG_IMMEDIATE) {
        return IMMEDIATE_MODE;
    }
    if (type == ARG_DIRECT) {
        return DIRECT_MODE;
    }
    if (type == ARG_MATRIX) {
        return MATRIX_MODE;
    }
    if (type == ARG_REGISTER) {
        return REGISTER_MODE;
    }
    return IMMEDIATE_MODE;
}

int get_symbol_address(SymbolTable* table, char* symbol_name) {
    Symbol* symbol = find_symbol(table, symbol_name);
    if (symbol) {
        return symbol->address;
    }
    return -1;
}

int validate_symbol_exists(SymbolTable* table, char* symbol_name, ErrorManager* error_mgr, int line) {
    int address = get_symbol_address(table, symbol_name);
    if (address < 0) {
        add_error(error_mgr, SYMBOL_NO_EXIST, line);
        return 0;
    }
    return 1;
}
