#ifndef AST_H
#define AST_H

/*
    AST node construction uses an OpCode enum to represent instruction types.
    Each opcode acts like a state in a simplified state machine, where the parser
    chooses how to handle the line based on the opcode and operands.
*/
/* basic AST structs */

typedef struct ASTNode ASTNode;

typedef enum {
 /*Instruction two Operands*/ OP_MOV,OP_CMP,OP_ADD,OP_SUB,OP_LEA,
/*Instruction one Operand*/   OP_CLR,OP_NOT,OP_INC,OP_DEC,OP_JMP,OP_BNE,OP_JSR,OP_RED,OP_PRN,
/*Instruction zero Operands*/ OP_RTS, OP_STOP,
 /*Directive*/ DIR_DATA,DIR_STRING,DIR_MAT,DIR_ENTRY,DIR_EXTERN
} OpCode;

typedef enum {
    ARG_IMMEDIATE,
    ARG_DIRECT,
    ARG_REGISTER,
    ARG_MATRIX,
    ARG_NONE
} ArgType;

typedef struct {
    int col_reg;
    int row_reg;
}Matrix;

typedef union {
    int imd_value;
    int reg_value;
    char* label;
    Matrix matrix;
 }ValOperand;

typedef struct {
    ArgType type;
    ValOperand value;
} Operand;

typedef struct {
    int address;
    char* label;
    char* original_line;
    int line_num;
} NodeInfo;

typedef struct {
    int data_size;                   /* how many words this node takes in memory */
    int* data_values;                /* for .data or .mat values */
    int data_count;                  /* how many values there are */
    char* string_value;             /* for string directives */
}DataInfo;

/* ASTNode node struct */
struct ASTNode { 
    /* essinial */
    OpCode opcode;
    NodeInfo info;
    /* Optional */
    Operand operands[2];
    DataInfo* data;
    ASTNode* next;            /* linked list of instructions/directives */
};

/* Node builder */
ASTNode* create_empty_node();
ASTNode* create_new_node(OpCode op, NodeInfo info);

/* setters */
void set_operand_node(ASTNode* node, int op_index, Operand operand);
void set_data_info(ASTNode* node, DataInfo* data);
void set_node_address(ASTNode* node, int address);

/* Operand builders */
Operand op_new_immediate(int val);
Operand op_new_register(int val);
Operand op_new_label(char* label);
Operand op_new_matrix(int row_reg, int col_reg);
Operand op_empty_operand();

void add_ast_node(ASTNode** head, ASTNode* new_node) ;

/* Memory freeing functions */
void free_operand(Operand* op);
void free_node(ASTNode* node);  
void free_data_info(DataInfo* data);
void free_ast_list(ASTNode* head);  /*freeing a list given it's "head ast"*/



#endif
