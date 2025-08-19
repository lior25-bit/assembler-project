/* tester 1 - lir part */

#include "constants.h"
#include "ast.h"
#include "ast_logic.h"
#include "first_pass.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "memory_manager.h"
#include "error_manager.h"
#include "file_export.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* local helpers */
static char* dupstr(const char* s) {
    char* p; size_t n;
    if (!s) return NULL;
    n = strlen(s) + 1;
    p = (char*)malloc(n);
    if (!p) return NULL;
    memcpy(p, s, n);
    return p;
}

static Operand op_imm(int v) {
    Operand o;
    o.type = ARG_IMMEDIATE;
    o.mat_label = NULL;
    o.val.value = v;
    return o;
}

static Operand op_reg(int r) {
    Operand o;
    o.type = ARG_REGISTER;
    o.mat_label = NULL;
    o.val.reg_value = r;
    return o;
}

static Operand op_label(const char* name) {
    Operand o;
    o.type = ARG_DIRECT;
    o.mat_label = NULL;
    o.val.label = (char*)dupstr(name);
    return o;
}

static Operand op_matrix(const char* name, int r_row, int r_col) {
    Operand o;
    o.type = ARG_MATRIX;
    o.mat_label = (char*)dupstr(name);
    o.val.matrix_regs[0] = r_row;
    o.val.matrix_regs[1] = r_col;
    return o;
}

static ASTNode* make_instr(const char* label, OpCode op, Operand src, Operand dst, const char* og) {
    ASTNode* n = (ASTNode*)calloc(1, sizeof(ASTNode));
    if (!n) return NULL;
    n->node_type = NODE_INSTRUCTION;
    n->opcode = op;
    n->operands[0] = src;
    n->operands[1] = dst;
    n->address = 0;
    n->label = label ? dupstr(label) : NULL;
    n->original_line = og ? dupstr(og) : NULL;
    n->next = NULL;
    return n;
}

static ASTNode* make_data(const char* label, const int* vals, int count, const char* og) {
    int i;
    ASTNode* n = (ASTNode*)calloc(1, sizeof(ASTNode));
    if (!n) return NULL;
    n->node_type = NODE_DIRECTIVE;
    n->label = label ? dupstr(label) : NULL;
    n->directive.dir_type = DIR_DATA;
    n->directive.value.data.count = count;
    if (count > 0 && vals) {
        n->directive.value.data.values = (int*)malloc(sizeof(int)*count);
        if (!n->directive.value.data.values) return n;
        for (i = 0; i < count; i++) n->directive.value.data.values[i] = vals[i];
    }
    n->original_line = og ? dupstr(og) : NULL;
    n->next = NULL;
    return n;
}

static ASTNode* make_string(const char* label, const char* s, const char* og) {
    ASTNode* n = (ASTNode*)calloc(1, sizeof(ASTNode));
    if (!n) return NULL;
    n->node_type = NODE_DIRECTIVE;
    n->label = label ? dupstr(label) : NULL;
    n->directive.dir_type = DIR_STRING;
    n->directive.value.string_val = s ? dupstr(s) : NULL;
    n->original_line = og ? dupstr(og) : NULL;
    n->next = NULL;
    return n;
}

static ASTNode* make_mat(const char* label, int rows, int cols, const int* vals, const char* og) {
    int i, cells;
    ASTNode* n = (ASTNode*)calloc(1, sizeof(ASTNode));
    if (!n) return NULL;
    n->node_type = NODE_DIRECTIVE;
    n->label = label ? dupstr(label) : NULL;
    n->directive.dir_type = DIR_MAT;
    n->directive.value.mat.rows = rows;
    n->directive.value.mat.cols = cols;
    cells = rows * cols;
    if (cells > 0 && vals) {
        n->directive.value.mat.values = (int*)malloc(sizeof(int)*cells);
        if (!n->directive.value.mat.values) return n;
        for (i = 0; i < cells; i++) n->directive.value.mat.values[i] = vals[i];
    }
    n->original_line = og ? dupstr(og) : NULL;
    n->next = NULL;
    return n;
}

static void free_ast(ASTNode* head) {
    ASTNode* cur = head;
    while (cur) {
        ASTNode* nx = cur->next;
        if (cur->label) free(cur->label);
        if (cur->original_line) free(cur->original_line);
        if (cur->node_type == NODE_DIRECTIVE) {
            if (cur->directive.dir_type == DIR_DATA && cur->directive.value.data.values)
                free(cur->directive.value.data.values);
            if (cur->directive.dir_type == DIR_STRING && cur->directive.value.string_val)
                free(cur->directive.value.string_val);
            if (cur->directive.dir_type == DIR_MAT && cur->directive.value.mat.values)
                free(cur->directive.value.mat.values);
        }
        if (cur->operands[0].type == ARG_DIRECT && cur->operands[0].val.label)
            free(cur->operands[0].val.label);
        if (cur->operands[1].type == ARG_DIRECT && cur->operands[1].val.label)
            free(cur->operands[1].val.label);
        if (cur->operands[0].type == ARG_MATRIX && cur->operands[0].mat_label)
            free(cur->operands[0].mat_label);
        if (cur->operands[1].type == ARG_MATRIX && cur->operands[1].mat_label)
            free(cur->operands[1].mat_label);
        free(cur);
        cur = nx;
    }
}

int main(void)
{
    MemoryManager* mem;
    SymbolTable*   symtab;
    ErrorManager   em;
    ASTNode *n1, *n2, *n3, *n4, *n5, *n6, *head, *tail;
    int data_vals[3];
    int mat_vals[4];
    Symbol* ext;

    initialize_error_manager(&em);

    mem = create_memory_manager();
    symtab = create_empty_table();
    if (!mem || !symtab) {
        printf("alloc failed\n");
        return 1;
    }

    /* build AST */
    n1 = make_instr("MAIN", OP_MOV, op_imm(5), op_reg(3), "MAIN: mov #5, r3");
    n2 = make_instr(NULL, OP_ADD, op_reg(2), op_reg(7), "add r2, r7");
    data_vals[0] = 7; data_vals[1] = -3; data_vals[2] = 5;
    n3 = make_data("LIST", data_vals, 3, "LIST: .data 7,-3,5");
    n4 = make_string("STR", "hi", "STR: .string \"hi\"");
    mat_vals[0] = 1; mat_vals[1] = 2; mat_vals[2] = 3; mat_vals[3] = 4;
    n5 = make_mat("MAT", 2, 2, mat_vals, "MAT: .mat [2][2] 1,2,3,4");
    n6 = make_instr(NULL, OP_JSR, op_label("EXTLBL"), empty_operand(), "jsr EXTLBL");

    head = n1; tail = n1;
    tail->next = n2; tail = n2;
    tail->next = n3; tail = n3;
    tail->next = n4; tail = n4;
    tail->next = n5; tail = n5;
    tail->next = n6; tail = n6;

    /* add extern symbol so second pass treats EXTLBL as extern */
    ext = create_symbol("EXTLBL", 0, SYMBOL_EXTERN);
    add_last_symbol(symtab, ext);

    /* mark entry to produce .ent */
    mark_entry(symtab, "MAIN");

    /* run passes */
    first_pass(head, mem, symtab, &em);
    second_pass(head, mem, symtab, &em);

    /* export files with base name "prog" */
    export_all_files("prog", mem, symtab, head);

    /* cleanup */
    free_memory_manager(mem);
    free_symbol_table(symtab);
    free_ast(head);

    return 0;
}
