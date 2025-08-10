/* file: ast.c
   description: AST code with functions used for building ast syntax tree.
   by: Lir Vine and Lior Tal.
*/
#include <stdlib.h>
#include "ast.h"
#include "ast_logic.h"
#include  "constants.h"
#include "error_manager.h"

/* builders */
/* This function creates an empty node */
ASTNode* create_empty_node() {
    ASTNode* node = calloc(1, sizeof(ASTNode)); /* calloc allocate memory and initilized it */
    if(!node) {
        /*error*/
        return NULL;
    }
 return node;
}

/* This function creates a new empty node given the essinial field only */
ASTNode* create_new_node(OpCode op, NodeInfo info) {
    ASTNode* node = create_empty_node();
    if(!node) {
        /*error - */
        return NULL;
    }
    node->opcode = op;
    node->info = info;
    
    return node;
}


/* node setters */
/* This function set operand to a given node */ 
void set_operand_node(ASTNode* node, int op_index, Operand operand) {
    if(!node) {
        /*error - null node*/
        return;
    }
    
    /*if(op_index < 0 || op_index > 1 ) {
    eroro invalid index operator
        return;
    } here using a function by fronterror */
    
  /*  if(node->operands[op_index].type != ARG_NONE) {
        error
        return; here will be function from froneteer*/

    node->operands[op_index] = operand;
}

/*This function set dataInfo to a given node */
void set_data_info(ASTNode* node, DataInfo* data) {
    if(!node) {
        /* error null node*/
        return;
    }
    if(node->data) {
        /* error data alre*/
        return;
    }

    node->data = data;
}

/* This function set an address to a given node */
void set_node_address(ASTNode* node, int address) {
    if(!node) {
        /*error*/
        return;
    }
    /*
    if(adress < MIN_ADDRESS || adress > MAX_ADDRESS) {
        ERROR INVALID ADRESS
       return
    }*/
    
    node->info.address = address;
}

/* Operands builders */
/* This function builds a new type Immidiate operand */
Operand op_new_immediate(int val) {
    Operand op;
   /* if(val < MIN_IMIDIATE || val > MAX_IMIDIATE) {
        error
       op.type = ARG_NONE;
    } */

    op.type = ARG_IMMEDIATE;
    op.value.imd_value = val;
    return op;
} 

/* This function builds a new type Register operand */
Operand op_new_register(int val) {
    Operand op;
    /*if(val < MIN_REGISTER || val > MAX_REGISTER) {
        error
        op.type = ARG_NONE; - frontend error
    } */
    op.type = ARG_REGISTER;
    op.value.reg_value = val;
    return op;
}

/* This function builds a new type direct operand */
Operand op_new_label(char* label) {
    Operand op;
   /* if(!isValidlabel(label)) {
        op.type = ARG_NONE;
    } fromend cheakit */
    op.type = ARG_DIRECT;
    op.value.label = label;
    return op;
}

/* This function buids new type matrix operand */
Operand op_new_matrix(int row_reg, int col_reg) {
    Operand op;
    /*front cheaking matrix okay if not i send error*/
    op.type = ARG_MATRIX;
    op.value.matrix.row_reg = row_reg;
    op.value.matrix.col_reg = col_reg;
    return op;
}

Operand op_empty_operand() {
    Operand op;
    op.type = ARG_NONE;
    return op;
}

void add_ast_node(ASTNode** head, ASTNode* new_node) {
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    ASTNode* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = new_node;
}

/* free */

/*this function */
void free_operand(Operand* op) {
    if(!op) {
        /*error*/
        return;
    }
    
    if(op->type == ARG_DIRECT && op->value.label) {
        free(op->value.label); /* we use free for a direct label since the memory could be allocaed by calloc/malloc */
        op->value.label = NULL;
    }
    op->type = ARG_NONE;
}

void free_node(ASTNode* node) {
    if(!node) {
        /*error*/
        return;
    }
    free_operand(&node->operands[0]);
    free_operand(&node->operands[1]);

    if(node->info.label) {
        free(node->info.label); 
        node->info.label = NULL;
    }
    if(node->info.original_line) {
        free(node->info.original_line); 
        node->info.original_line = NULL;        
    }
    if(node->data) {
        free_data_info(node->data);
        node->data = NULL;
    }

}

void free_data_info(DataInfo* data) {
    if(!data) {
        /*error*/
        return;
    }
    if(data->data_values) {
        free(data->data_values);
        data->data_values = NULL;
    }
    if (data->string_value) {
        free(data->string_value); 
        data->string_value = NULL;
    }
    free(data);
}

void free_ast_list(ASTNode* head) {
    ASTNode* current = head;
    while(current != NULL) {
        ASTNode* temp = current;
        current = current->next;
        free_node(temp);
        free(temp);
    }
}


