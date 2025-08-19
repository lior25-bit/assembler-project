
/* This file includes all the functions needed for the second pass */
#include "second_pass.h"
#include "ast_logic.h"
#include "symbol_logic.h"
#include <string.h>

static int ic_cursor = 0;
static int dc_cursor = 0;

static int encode_operand_internal(Operand operand, MemoryManager* memory, SymbolTable* table, int is_source, ErrorManager* error_mgr, const char* og_line);

/* This function performs the second pass of the assembler, encoding all instructions and data into machine code. */
int second_pass(ASTNode* ast_head, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    ASTNode* cur;

    if (!memory) return 0;

    ic_cursor = MEMORY_START;
    dc_cursor = 0;

    cur = ast_head;
    while (cur) {
        process_node_second_pass(cur, memory, table, error_mgr);
        cur = cur->next;
    }

    print_all_errors(error_mgr);
    return 1;
}

/* This functions process each node for the second pass */
int process_node_second_pass(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    if (!node || !memory || !table) return 0;

    if (is_directive_node(node)) {
        return process_directive_second_pass(node, memory, error_mgr);
    }
    if (is_instruction_node(node)) {
        return process_instruction_second_pass(node, memory, table, error_mgr);
    }
    return 0;
}

/* This function process directive nodes */
int process_directive_second_pass(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    if (!node || !memory) return 0;

    switch (node->directive.dir_type) {
    case DIR_ENTRY:
        return 1;
    case DIR_EXTERN:
        return 1;
    case DIR_DATA:
        return encode_data_directive(node, memory, error_mgr);
    case DIR_STRING:
        return encode_string_directive(node, memory, error_mgr);
    case DIR_MAT:
        return encode_mat_directive(node, memory, error_mgr);
    default:
        add_error(error_mgr, UNKNOWN_OPCODE, node->original_line);
        return 0;
    }
}

/* This function handle entrt directive nodes */
int handle_entry_directive(ASTNode* node, SymbolTable* table, ErrorManager* error_mgr) {
    (void)node;
    (void)table;
    (void)error_mgr;
    return 1;
}

/* This function encode data directive nodes */
int encode_data_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    int i;
    int n;
    int* arr;

    if (!node || !memory) return 0;

    n = node->directive.value.data.count;
    arr = node->directive.value.data.values;

    if (n < 0) {
        add_error(error_mgr, INVALID_OPCODE, node->original_line);
        return 0;
    }
    for (i = 0; i < n; i++) {
        if (dc_cursor >= MAX_MEMORY) {
            add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
            return 0;
        }
        memory->data_image[dc_cursor++] = arr ? arr[i] : 0;
    }
    return 1;
}

/* This function incodes string directive nodes */
int encode_string_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    const char* s;
    int i;

    if (!node || !memory) return 0;

    s = node->directive.value.string_val;
    if (!s) {
        add_error(error_mgr, INVALID_OPCODE, node->original_line);
        return 0;
    }
    for (i = 0; s[i] != '\0'; i++) {
        if (dc_cursor >= MAX_MEMORY) {
            add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
            return 0;
        }
        memory->data_image[dc_cursor++] = (unsigned char)s[i];
    }
    if (dc_cursor >= MAX_MEMORY) {
        add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
        return 0;
    }
    memory->data_image[dc_cursor++] = 0;
    return 1;
}

/* This function encodes matrix directive nodes */
int encode_mat_directive(ASTNode* node, MemoryManager* memory, ErrorManager* error_mgr) {
    int cells;
    int i;
    int* vals;

    if (!node || !memory) return 0;

    if (node->directive.value.mat.rows <= 0 || node->directive.value.mat.cols <= 0) {
        add_error(error_mgr, INVALID_OPCODE, node->original_line);
        return 0;
    }

    cells = node->directive.value.mat.rows * node->directive.value.mat.cols;
    vals  = node->directive.value.mat.values;

    for (i = 0; i < cells; i++) {
        if (dc_cursor >= MAX_MEMORY) {
            add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
            return 0;
        }
        memory->data_image[dc_cursor++] = vals ? vals[i] : 0;
    }
    return 1;
}

/* This function process instruction nodes */
int process_instruction_second_pass(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    if (!node || !memory) return 0;
    return encode_instruction(node, memory, table, error_mgr);
}

/* This function encodes instruction nodes */
int encode_instruction(ASTNode* node, MemoryManager* memory, SymbolTable* table, ErrorManager* error_mgr) {
    int opcode;
    int srcm;
    int destm;
    int word;
    int both_regs;
    Operand src;
    Operand dst;

    if (!node || !memory) return 0;

    src = node->operands[0];
    dst = node->operands[1];

    opcode = node->opcode;
    srcm   = get_addressing_mode_code(src.type);
    destm  = get_addressing_mode_code(dst.type);

    word = create_machine_word(opcode, srcm, destm, ARE_ABSOLUTE);
    if (ic_cursor < 0 || ic_cursor >= MAX_MEMORY) {
        add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
        return 0;
    }
    write_word_to_memory(memory, ic_cursor, word, ARE_ABSOLUTE);
    ic_cursor++;

    both_regs = (src.type == ARG_REGISTER && dst.type == ARG_REGISTER);

    if (both_regs) {
        int regw;
        regw = 0;
        regw |= ((src.val.reg_value & REGISTER_MASK)  << SRC_REG_SHIFT);
        regw |= ((dst.val.reg_value & REGISTER_MASK)  << DEST_REG_SHIFT);
        if (ic_cursor >= MAX_MEMORY) {
            add_error(error_mgr, MEMORY_OVERFLOW, node->original_line);
            return 0;
        }
        write_word_to_memory(memory, ic_cursor, regw, ARE_ABSOLUTE);
        ic_cursor++;
        return 1;
    }

    if (src.type != ARG_NONE) {
        if (!encode_operand_internal(src, memory, table, 1, error_mgr, node->original_line)) return 0;
    }
    if (dst.type != ARG_NONE) {
        if (!encode_operand_internal(dst, memory, table, 0, error_mgr, node->original_line)) return 0;
    }
    return 1;
}

/* This function encode operand */
int encode_operand(Operand operand, MemoryManager* memory, SymbolTable* table, int is_source, ErrorManager* error_mgr) {
    return encode_operand_internal(operand, memory, table, is_source, error_mgr, 0);
}

/* This function encodes iternal operand */
static int encode_operand_internal(Operand operand, MemoryManager* memory, SymbolTable* table, int is_source, ErrorManager* error_mgr, const char* og_line) {
    int w;
    Symbol* sym;

    if (operand.type == ARG_IMMEDIATE) {
        if (ic_cursor >= MAX_MEMORY) {
            add_error(error_mgr, MEMORY_OVERFLOW, og_line);
            return 0;
        }
        w = (operand.val.value & VALUE_MASK);
        write_word_to_memory(memory, ic_cursor, w, ARE_ABSOLUTE);
        ic_cursor++;
        return 1;
    }

    if (operand.type == ARG_DIRECT) {
        sym = find_symbol(table, operand.val.label);
        if (!sym) {
            add_error(error_mgr, SYMBOL_NO_EXIST, og_line);
            return 0;
        }
        if (sym->type == SYMBOL_EXTERN) {
            if (ic_cursor >= MAX_MEMORY) {
                add_error(error_mgr, MEMORY_OVERFLOW, og_line);
                return 0;
            }
            write_word_to_memory(memory, ic_cursor, 0, ARE_EXTERNAL);
            add_extern_usage(memory, operand.val.label, ic_cursor);
            ic_cursor++;
            return 1;
        } else {
            if (ic_cursor >= MAX_MEMORY) {
                add_error(error_mgr, MEMORY_OVERFLOW, og_line);
                return 0;
            }
            write_word_to_memory(memory, ic_cursor, (sym->address & VALUE_MASK), ARE_RELOCATABLE);
            ic_cursor++;
            return 1;
        }
    }

    if (operand.type == ARG_REGISTER) {
        if (ic_cursor >= MAX_MEMORY) {
            add_error(error_mgr, MEMORY_OVERFLOW, og_line);
            return 0;
        }
        w = 0;
        if (is_source) {
            w |= ((operand.val.reg_value & REGISTER_MASK) << SRC_REG_SHIFT);
        } else {
            w |= ((operand.val.reg_value & REGISTER_MASK) << DEST_REG_SHIFT);
        }
        write_word_to_memory(memory, ic_cursor, w, ARE_ABSOLUTE);
        ic_cursor++;
        return 1;
    }

    if (operand.type == ARG_MATRIX) {
        int idxw;

        sym = find_symbol(table, operand.mat_label);
        if (!sym) {
            add_error(error_mgr, SYMBOL_NO_EXIST, og_line);
            return 0;
        }
        if (ic_cursor >= MAX_MEMORY) {
            add_error(error_mgr, MEMORY_OVERFLOW, og_line);
            return 0;
        }
        if (sym->type == SYMBOL_EXTERN) {
            write_word_to_memory(memory, ic_cursor, 0, ARE_EXTERNAL);
            add_extern_usage(memory, operand.mat_label, ic_cursor);
        } else {
            write_word_to_memory(memory, ic_cursor, (sym->address & VALUE_MASK), ARE_RELOCATABLE);
        }
        ic_cursor++;

        if (ic_cursor >= MAX_MEMORY) {
            add_error(error_mgr, MEMORY_OVERFLOW, og_line);
            return 0;
        }
        idxw = 0;
        idxw |= ((operand.val.matrix_regs[0] & REGISTER_MASK) << SRC_REG_SHIFT);
        idxw |= ((operand.val.matrix_regs[1] & REGISTER_MASK) << DEST_REG_SHIFT);
        write_word_to_memory(memory, ic_cursor, idxw, ARE_ABSOLUTE);
        ic_cursor++;

        return 1;
    }

    return 1;
}

/* This function get the symbol address */
int get_symbol_address(SymbolTable* table, char* symbol_name) {
    Symbol* s;
    if (!table || !symbol_name) return -1;
    s = find_symbol(table, symbol_name);
    if (!s) return -1;
    return s->address;
}

/* This function validates symbol exsits */
int validate_symbol_exists(SymbolTable* table, char* symbol_name, ErrorManager* error_mgr, int line) {
    (void)line;
    if (get_symbol_address(table, symbol_name) < 0) {
        add_error(error_mgr, SYMBOL_NO_EXIST, 0);
        return 0;
    }
    return 1;
}

/* This function create the word in order */
int create_machine_word(int opcode, int src_mode, int dest_mode, int are) {
    int w = 0;
    w |= ((opcode    & OPCODE_MASK)          << OPCODE_SHIFT);
    w |= ((src_mode  & ADDRESSING_MODE_MASK) << SRC_MODE_SHIFT);
    w |= ((dest_mode & ADDRESSING_MODE_MASK) << DEST_MODE_SHIFT);
    w |= (are & 0x3);
    return w;
}

/* This function writes a word to memory */
void write_word_to_memory(MemoryManager* memory, int address, int word, int are) {
    int v;
    if (!memory) return;
    if (address < 0 || address >= MAX_MEMORY) return;
    v = word;
    v &= ~0x3;
    v |= (are & 0x3);
    memory->code_image[address] = v;
}

/* This function gets the addressing mode code for a given argument type */
int get_addressing_mode_code(ArgType type) {
    switch (type) {
    case ARG_IMMEDIATE: return IMMEDIATE_MODE;
    case ARG_DIRECT:    return DIRECT_MODE;
    case ARG_MATRIX:    return MATRIX_MODE;
    case ARG_REGISTER:  return REGISTER_MODE;
    default:            return 0;
    }
}

/* This function calculates the number of words needed for a given operand */
int calculate_operand_words(Operand operand) {
    if (operand.type == ARG_NONE) return 0;
    if (operand.type == ARG_MATRIX) return 2;
    return 1;
}
