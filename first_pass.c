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
    int address;

    if (node->label != NULL) {
        if (!process_label(node)) return 0;
    }

    if (is_extern_directive(node)) {
        add_symbol(node->operands[0].string_value, 0, SYMBOL_EXTERN);
        return 0; 
    }

    SymbolType type = process_node_type(node);
    address = (type == SYMBOL_DATA) ? memory->DC : memory->IC;

    if (node->label != NULL && !is_entry(node) && !is_extern_directive(node)) {
        add_symbol(node->label, address, type);
    }

    if (is_directive(node)) {
        memory->DC += node->data_size;
    } else {
        memory->IC += calculate_instruction_size(node);
    }

    return 0;
}


SymbolType process_node_type(ASTNode* node) {
    if (is_extern_directive(node)) {
        return SYMBOL_EXTERN;
    } 
    else if (is_directive(node)) {
        return SYMBOL_DATA;
    } 
    else {
        return SYMBOL_CODE;
    }
}


int process_label(ASTNode* node) {
    /* is the label itself defiend properly */
    if (!is_valid_label(node->label)) {
        return 0;  
    }

    if (is_entry(node) || is_extern_directive(node)) {
       /* lable should not contain entry or extern symbol */
        label_on_extern_error(node->label); 
        return 0;
    }

    return 1;  
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


int is_string_directive(ASTNode* node) {
    return node->opcode == DIR_STRING;
}

int is_mat_directive(ASTNode* node) {
    return node->opcode == DIR_MAT;
}

int is_reserved_word(const char* name) {
    /* Array that contains all the the reserved words */
     const char* reserved_list[] = {
    /* Opcodes */
    "mov", "cmp", "add", "sub", "lea",
    "clr", "not", "inc", "dec", 
    "jmp", "bne", "jsr", "red", "prn", "rts", "stop", 
    /* Directives */
    "data", "string", "mat", "entry", "extern", 
    /* Registers */
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
    };
    int count = sizeof(reserved_list) / sizeof(reserved_list[0]); /* Number of elements in reserved_list */
    for(int i=0; i < count; i++) {
        if(strcmp(name, reserved_list[i]) == 0) {
            return 1; /* Word is a reserved word */
        }
    }
    return 0;
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

int is_valid_label(const char* label) {
    if(label == NULL) {
        no_label_error(label);
        return 0;
    }
    size_t length_label = strlen(label);

    if(length_label == 0) {
        empty_label_error(label);
        return 0;
    }

    if(is_reserved_word(label)) {
        reserved_word_label_error(label);
        return 0;
    }

     if(!isalpha(label[0])) {
        illigal_start_label_error(label);
        return 0;
    }

    if(length_label > MAX_LABEL) {
        label_too_long_error(label);
        return 0;
    }

    for(int i=0; label[i] != '\0'; i++) {
        if(!isalpha(label[i]) && !isdigit(label[i])) {
            illigal_label(label);
            return 0;
        }
    }
   return 1;
}



