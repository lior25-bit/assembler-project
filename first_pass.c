#include "first_pass.h"
#include "symbol_table.h"
#include "middle_error.h"
#include "ast.h"


/* IC = כמה מילות זיכרון תופסות כל ההוראות
   DC = כמה מילות זיכרון תופסים כל הנתונים */

int first_pass(ASTNode* ast_head, MemoryManager* memory) {
    memory->IC = 0;
    memory->DC = 0;

    ASTNode* current = ast_head;
    while (current != NULL) {
        process_node(current, memory);
        current = current->next;
    }

    update_data_symbols(symbol_table_head, memory->IC);

    return 0;
}

/* Process a single AST node and update memory + symbol table */
int process_node(ASTNode* node, MemoryManager* memory) {
    int address = 0;

    if (node->label != NULL) {
        SymbolType type;

        if (is_extern_directive(node)) {
            type = SYMBOL_EXTERN;
        } else if (is_directive(node)) {
            type = SYMBOL_DATA;
        } else {
            type = SYMBOL_CODE;
        }

        address = (type == SYMBOL_DATA) ? memory->DC : memory->IC + MEMORY_START;
        add_symbol(node->label, address, type);
    }

    if (is_directive(node)) {
        memory->DC += node->data_size;
    } else {
        memory->IC += calculate_instruction_size(node);
    }

    return 0;
}

/* Return the number of words the instruction occupies */
int calculate_instruction_size(ASTNode* node) {
    int size = 1;

    Operand src = node->operands[0];
    Operand dest = node->operands[1];

    switch (node->opcode) {
        case OP_MOV:
        case OP_CMP:
        case OP_ADD:
        case OP_SUB:
        case OP_LEA:
            if (src.type == ARG_REGISTER && dest.type == ARG_REGISTER)
                size += 1;
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
            // כאן את יכולה להוסיף הודעת שגיאה או אזהרה על אופקוד לא חוקי
            break;
    }

    return size;
}

/* בדיקה אם הפקודה היא הנחיה (.data / .string וכו') */
int is_directive(ASTNode* node) {
    return (node->opcode >= DIR_DATA && node->opcode <= DIR_EXTERN);
}

/* בדיקה אם הפקודה היא specifically .extern */
int is_extern_directive(ASTNode* node) {
    return node->opcode == DIR_EXTERN;
}

/* האם מדובר בהנחיה מסוג .entry */
int is_entry(ASTNode* node) {
    return node->opcode == DIR_ENTRY;
}

/* האם הפקודה היא .string */
int is_string_directive(ASTNode* node) {
    return node->opcode == DIR_STRING;
}

/* האם הפקודה היא .mat */
int is_mat_directive(ASTNode* node) {
    return node->opcode == DIR_MAT;
}

/* מעדכן את הכתובות של סמלים מסוג DATA */
void update_data_symbols(Symbol* head, int ic) {
    Symbol* current = head;
    while (current != NULL) {
        if (current->type == SYMBOL_DATA) {
            current->address += ic;
        }
        current = current->next;
    }
}
