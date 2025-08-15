#ifndef AST_H
#define AST_H
#include "frontend_error.h"
#include "lexer.h"
/*
In this project we want to 'handle' the files by creating AST node that will represent everything : labels, instructions, directive, ect...
In order to do that we create this AST. 
*/

/*enum that represent every instruction that there are, 16*/
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
    OP_STOP,
    OP_NONE
} OpCode;

/*enum that checks if the line is directive line or unstruction line*/
typedef enum {
    NODE_INSTRUCTION,
    NODE_DIRECTIVE, 
    NODE_ERROR
} NodeType;

/*enum that checks which operand we have in this line*/
typedef enum {
    ARG_IMMEDIATE,
    ARG_DIRECT,
    ARG_REGISTER,
    ARG_MATRIX,
    ARG_NONE
} ArgType;

/*enum that checks which directive type we have in this line */
typedef enum {
    DIR_DATA,     /* .data */
    DIR_STRING,   /* .string */
    DIR_MAT,      /* .mat */
    DIR_EXTERN,   /* .extern */
    DIR_ENTRY,     /* .entry */
    DIR_NONE
} DirectiveType;

/*In case of an instruction line*/
typedef struct {
    ArgType type;
    char *mat_label; 
    union {
        int   value;          /* ARG_IMMEDIATE */
        int   reg_value;      /* ARG_REGISTER  */
        char *label;          /* ARG_DIRECT    */
        int   matrix_regs[2]; /* ARG_MATRIX    */
    } val;
} Operand;

/*In case of a directive line*/
typedef struct {
    DirectiveType dir_type;
    char* label; 
    union {
        struct DataVal {
            int* values;
            int count;
        } data;      
        char* string_val;                              
        struct MatVal {
            int rows, cols;
            int* values;
        } mat;   
        char* ext_label;                          
    } value;
} DirectiveInfo;

/*The overall AST node that  we create for each line, supported by all 
the other structs and enums that represent directive of instruction line*/
typedef struct ASTNode {
    NodeType        node_type;
    OpCode          opcode;
    Operand         operands[2];
    int             address;
    char           *label;
    char           *original_line;
    DirectiveInfo   directive;
    FrontErrorInfo *error_info;
    struct ASTNode *next;  /* עכשיו זה אותו טיפוס כמו ASTNode* */
} ASTNode;
/* Operand builders */
Operand new_immediate(int val);
Operand new_register(int val);
Operand new_label(const char* input);
Operand new_matrix(int row_reg, int col_reg);
Operand empty_operand(void);
/* AST node builder */
ASTNode* build_directive_node(DirectiveType dir_type, const char* label,const void* value,int count_or_rows,int cols, const char* original_line) ;
ASTNode* new_node(OpCode op, Operand op1, Operand op2, int new_address, char* new_label, char* og_line);
ASTNode* build_error_node(const FrontErrorInfo* err_info, const char* original_line);
Operand build_operand_full(const Token* tokens_arr, int *idx, int num_tokens);
OpCode opcode_from_text(const char* str);
/* Debug printing */
void print_operand(Operand op);
void print_opcode(ASTNode* node);
void print_label(ASTNode* node);
void print_ast_node(ASTNode* node);
#endif