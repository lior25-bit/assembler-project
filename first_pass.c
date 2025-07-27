#include "first_pass.h"
#include "symbol_table.h"
#include "ast.h"
#include <string.h>

/* IC = כמה מילות זיכרון תופסות כל ההוראות
   DC = כמה מילות זיכרון תופסים כל הנתונים */

int first_pass(ASTNode* ast_head, MemoryManager* memory) {
    memory->IC = 0;
    memory->DC = 0;

    ASTNode* current = ast_head;
    while(current != NULL) {
        process_node(current, memory);
        current = current->next;
    }

    update_data_symbols(symbol_table_head ,memory->IC);

    return 0;
}

int process_node(ASTNode* node, MemoryManager* memory) {
    int is_data = 0;
    int is_extern = 0;
    int address = 0;

    if(node->label != NULL) {
        is_data = is_directive(node);              /* האם הנחיה */
        is_extern = is_extern_directive(node);     /* האם חיצונית */

        if(is_data) {
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

/*בשלב הזה לפני שכותבים את המעבר השני אי אפשר לחשב - בנתיים נחזיר 1 */
int calculate_instruction_size(ASTNode* node) {
    return 1; 
}

/* הפונקציה מקבלת צומת ומחזירה האם מכיל הנחיה */
int is_directive(ASTNode* node) {
    switch (node->opcode) {
        case DIR_DATA: return 1;
        case DIR_ENTRY: return 1;
        case DIR_EXTERN: return 1;
        case DIR_STRING: return 1;
        case DIR_MAT: return 1;
        default: return 0;
    }
}

/*האם אקסטרן הנחיה */
int is_extern_directive(ASTNode* node) {
    return node->opcode == DIR_EXTERN;
}

/*מחיר את הגודל של ההוראות (כמה)*/
int calculate_instruction_size(ASTNode* node) {
    int size = 1;  // תמיד יש מילה אחת לפקודה עצמה

    Operand src = node->operands[0];
    Operand dest = node->operands[1];

    /* פקודות עם שני אופרנדים */
    switch (node->opcode) {
        case OP_MOV:
        case OP_CMP:
        case OP_ADD:
        case OP_SUB:
        case OP_LEA:
            if (src.type == ARG_REGISTER && dest.type == ARG_REGISTER)
                size += 1; // שני רגיסטרים במילה אחת
            else {
                if (src.type == ARG_MATRIX) size += 3;
                else if (src.type != ARG_NONE) size += 1;

                if (dest.type == ARG_MATRIX) size += 3;
                else if (dest.type != ARG_NONE) size += 1;
            }
            break;

        /* פקודות עם אופרנד אחד (רק DEST) */
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

        /* פקודות בלי אופרנדים */
        case OP_RTS:
        case OP_STOP:
            break;

        /* הנחיות - לא מחשבים כאן */
        default:
            break;
    }

    return size;
}

void update_data_symbols(Symbol* head, int ic) {
    Symbol* current = head;
    while (current != NULL) {
        if (current->is_data && !current->is_extern) {
            current->address += ic;
        }
        current = current->next;
    }
}

