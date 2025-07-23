#ifndef AST_H
#define AST_H

/*
    AST node construction uses an OpCode enum to represent instruction types.
    Each opcode acts like a state in a simplified state machine, where the parser
    chooses how to handle the line based on the opcode and operands.
*/

typedef enum {
    OP_MOV,
    OP_CMP,
    OP_ADD,
    OP_SUB,
    OP_LEA,
    OP_CLR,
    OP_NOT,
    OP_INC,
    OP_DEC,
    OP_JMP,
    OP_BNE,
    OP_JSR,
    OP_RED,
    OP_PRN,
    OP_RTS,
    OP_STOP

/*הנחיות*/
   DIR_DATA,
   DIR_STRING,
   DIR_MAT,
   DIR_ENTRY,
   DIR_EXTERN

} OpCode;

typedef enum {
    ARG_IMMEDIATE,
    ARG_DIRECT,
    ARG_REGISTER,
    ARG_MATRIX,
    ARG_NONE
} ArgType;

typedef struct {
    ArgType type;

    union {
        int value;
        int reg_value;
        char* label;
        int matrix_regs[2]; 
    };

}Operand;

typedef struct ASTNode {

    OpCode opcode;
    Operand operands[2];
    int address;
    char* label;
    char* original_line;
    struct ASTNode* next; /*שינוי כדיי להפוך לרשימה מקושרת*/
}ASTNode;

/* Operand builders */
Operand new_immediate(int val);
Operand new_register(int val);
Operand new_label(char* input);
Operand new_matrix(int row_reg, int col_reg);
Operand empty_operand(void);

/* AST node builder */
ASTNode* new_node(OpCode op, Operand op1, Operand op2, int new_address, char* new_label, char* og_line);

/* Memory freeing functions */
void free_operand(Operand* op);
void free_label(ASTNode* node);
void free_node(ASTNode* node);

/* Debug printing */
void print_operand(Operand op);
void print_opcode(ASTNode* node);
void print_label(ASTNode* node);
void print_ast_node(ASTNode* node);




#endif /* AST_H */
