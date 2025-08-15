#include "ast.h"
#include <stdlib.h>   
#include <string.h>   
#include <stdio.h>
#include "lexer.h"

/*this is a helping function to copy string to a new place in memmory
 without the need to repeat this code all the tme*/
static char* dup_string(const char* s)
{
    char* out;
    size_t n;
    if (!s) return NULL;
    n = strlen(s) + 1;
    out = (char*)malloc(n);
    if (out) {
        memcpy(out, s, n);
    }
    return out;
}

/*building the immediate operand*/
Operand new_immediate(int val) {
    Operand op;
    op.type = ARG_IMMEDIATE;
    op.val.value = val;
    op.mat_label = NULL; 
    return op;
}
/*building the register operand*/
Operand new_register(int val) {
    Operand op;
    op.type = ARG_REGISTER;
    op.val.reg_value = val;
    op.mat_label = NULL; 
    return op;
}
/*building the inner label(as mat name) operand*/
Operand new_label(const char* input) {
    Operand op;
    op.type = ARG_DIRECT;
    op.val.label = dup_string(input); 
    op.mat_label = NULL;             
    return op;
}
/*building the mat operand*/
Operand new_matrix(int row_reg, int col_reg) {
    Operand op;
    op.type = ARG_MATRIX;
    op.val.matrix_regs[0] = row_reg;
    op.val.matrix_regs[1] = col_reg;
    op.mat_label = NULL;
    return op;
}
/*building the empty operand, for instruction without one or two operands*/
Operand empty_operand(void) {
    Operand op;
    op.type = ARG_NONE;
    op.mat_label = NULL;
    op.val.value = 0;
    return op;
}

/*this function biulds new node, this builder is for instruction lines*/
ASTNode* new_node(OpCode op, Operand op1, Operand op2,int new_address, char* new_label, char* og_line)
{
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->node_type = NODE_INSTRUCTION; /* instruction line*/
    node->opcode = op;
    node->operands[0] = op1;
    node->operands[1] = op2;
    node->address = new_address;

    node->label = dup_string(new_label);
    node->original_line = dup_string(og_line);

    node->directive.dir_type = DIR_NONE;/*without directive */
    node->error_info = NULL;
    node->next = NULL;
    return node;
}

/*this function biulds new node, this builder is for error lines*/
ASTNode* build_error_node(const FrontErrorInfo* err_info, const char* original_line) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->node_type = NODE_ERROR;/*error type*/
    node->opcode = 0;
    node->operands[0] = empty_operand();
    node->operands[1] = empty_operand();
    node->address = -1;
    node->label = NULL;
    node->original_line = dup_string(original_line);
    node->directive.dir_type = DIR_NONE;

    if (err_info) {
        node->error_info = (FrontErrorInfo*)malloc(sizeof(FrontErrorInfo));
        if (node->error_info) {
            memcpy(node->error_info, err_info, sizeof(FrontErrorInfo));
        }
    } else {
        node->error_info = NULL;
    }
    node->next = NULL;
    return node;
}

/*this function builds new node, this builder is for directive lines*/
ASTNode* build_directive_node(DirectiveType dir_type, const char* label,
                              const void* value, int count_or_rows, int cols,
                              const char* original_line)
{
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    int i, total;
    if (!node) return NULL;

    node->node_type = NODE_DIRECTIVE;
    node->opcode = 0;
    node->operands[0] = empty_operand();
    node->operands[1] = empty_operand();
    node->address = 0;
    node->label = NULL;
    node->original_line = dup_string(original_line);
    node->directive.dir_type = dir_type;
    node->directive.label = dup_string(label);
    node->error_info = NULL;
    node->next = NULL;

    switch (dir_type) {
        case DIR_DATA:/*if the directive is .data*/
            node->directive.value.data.count = count_or_rows;
            node->directive.value.data.values =(int*)malloc(sizeof(int) * (size_t)count_or_rows);
            if (!node->directive.value.data.values) break;
            for (i = 0; i < count_or_rows; ++i) {
                node->directive.value.data.values[i] =
                    ((const int*)value)[i];
            }
            break;

        case DIR_STRING:/*if the directive is .string*/
            node->directive.value.string_val =
                dup_string((const char*)value);
            break;

        case DIR_MAT:/*id the directive is .mat*/
            node->directive.value.mat.rows = count_or_rows;
            node->directive.value.mat.cols = cols;
            total = count_or_rows * cols;
            node->directive.value.mat.values =(int*)malloc(sizeof(int) * (size_t)total);
            if (!node->directive.value.mat.values) break;
            for (i = 0; i < total; ++i) {
                node->directive.value.mat.values[i] =
                    ((const int*)value)[i];
            }
            break;

        case DIR_ENTRY:/*if the directive is .entry*/
        case DIR_EXTERN:/*if the directive is .extern*/
            node->directive.value.ext_label =
                dup_string((const char*)value);
            break;

        default:
            if (node->directive.label) free(node->directive.label);
            if (node->original_line) free(node->original_line);
            free(node);
            return NULL;
    }

    return node;
}

/*this function will help us between the lexer and parser part, 
to convert a token to an operand*/
Operand build_operand_from_token(const Token* token) {
    Operand op;

    if (token == NULL) {
        return empty_operand();
    }

    switch (token->type) {
        case TOK_NUMBER:/*if the token is number*/
            if (token->text[0] == '#')
                op = new_immediate(atoi(token->text + 1));
            else
                op = new_immediate(atoi(token->text));
            break;

        case TOK_REGISTER:/*if the token is register*/
            op = new_register(atoi(token->text + 1));
            break;

        case TOK_LABEL:/*if the token is inner label*/
            op = new_label(token->text);
            break;

        case TOK_MATRIX_OP:/*if the token is met*/
            op = new_matrix(atoi(token->row_register + 1),
                            atoi(token->col_register + 1));
            if (token->matrix_name[0] != '\0') {
                op.mat_label = dup_string(token->matrix_name);
            } else {
                op.mat_label = NULL;
            }
            break;

        default:
            op = empty_operand();
            break;
    }

    return op;
}

/* another operand builder for different cases*/
Operand build_operand_full(const Token* a, int *idx, int len) {
    int i = *idx;
    Operand op;

    if (i >= len) return empty_operand();
    /*if the operand is a mat we make sure its valid */
    if (i + 6 < len &&
        a[i].type     == TOK_LABEL &&
        a[i+1].type   == TOK_SEPARATOR && a[i+1].text[0] == '[' &&
        a[i+2].type   == TOK_REGISTER &&
        a[i+3].type   == TOK_SEPARATOR && a[i+3].text[0] == ']' &&
        a[i+4].type   == TOK_SEPARATOR && a[i+4].text[0] == '[' &&
        a[i+5].type   == TOK_REGISTER &&
        a[i+6].type   == TOK_SEPARATOR && a[i+6].text[0] == ']')
    {
        op = new_matrix(atoi(a[i+2].text + 1), atoi(a[i+5].text + 1));
        op.mat_label = dup_string(a[i].text);
        *idx = i + 7;
        return op;
    }

    /* if the operand is immediate */
    if (a[i].text[0] == '#' && (i+1) < len && a[i+1].type == TOK_NUMBER) {
        op = new_immediate(atoi(a[i+1].text));
        *idx = i + 2;
        return op;
    }
    if (a[i].type == TOK_NUMBER && a[i].text[0] == '#') {
        op = new_immediate(atoi(a[i].text + 1));
        *idx = i + 1;
        return op;
    }

    /*if the operand is regular number*/
    if (a[i].type == TOK_NUMBER) {
        op = new_immediate(atoi(a[i].text));
        *idx = i + 1;
        return op;
    }

    /* if the operand is register*/
    if (a[i].type == TOK_REGISTER) {
        op = new_register(atoi(a[i].text + 1));
        *idx = i + 1;
        return op;
    }
    /* if the operand is inner label*/
    if (a[i].type == TOK_LABEL) {
        op = new_label(a[i].text);
        *idx = i + 1;
        return op;
    }
    return empty_operand();
}


/*function to free memorry*/
void free_operand(Operand* op) {
    if (!op) return;

    if (op->type == ARG_DIRECT && op->val.label) {
        free(op->val.label);
        op->val.label = NULL;
    }
    if (op->type == ARG_MATRIX && op->mat_label) {
        free(op->mat_label);
        op->mat_label = NULL;
    }
    op->type = ARG_NONE;
}
/*free label memmory*/
void free_label(ASTNode* node) {
    if (node && node->label) {
        free(node->label);
        node->label = NULL;
    }
}
/*free the whole node memmory*/
void free_node(ASTNode* node) {
    if (!node) return;

    if (node->node_type == NODE_DIRECTIVE) {
        if (node->directive.label) free(node->directive.label);

        if (node->directive.dir_type == DIR_DATA &&
            node->directive.value.data.values) {
            free(node->directive.value.data.values);
        }
        if (node->directive.dir_type == DIR_STRING &&
            node->directive.value.string_val) {
            free(node->directive.value.string_val);
        }
        if (node->directive.dir_type == DIR_MAT &&
            node->directive.value.mat.values) {
            free(node->directive.value.mat.values);
        }
        if ((node->directive.dir_type == DIR_EXTERN ||
             node->directive.dir_type == DIR_ENTRY) &&
            node->directive.value.ext_label) {
            free(node->directive.value.ext_label);
        }
    }

    free_label(node);
    free_operand(&node->operands[0]);
    free_operand(&node->operands[1]);
    if (node->original_line) {
        free(node->original_line);
        node->original_line = NULL;
    }
    if (node->error_info) {
        free(node->error_info);
        node->error_info = NULL;
    }
    free(node);
}


void print_operand(Operand op) {
    switch (op.type) {
        case ARG_IMMEDIATE:
            printf("Immediate: #%d\n", op.val.value);
            break;
        case ARG_REGISTER:
            printf("Register: R%d\n", op.val.reg_value);
            break;
        case ARG_DIRECT:
            printf("Label: %s\n", op.val.label ? op.val.label : "(null)");
            break;
        case ARG_MATRIX:
            /* מציגים גם שם מטריצה אם יש, וגם את הזוג [rx][ry] */
            printf("Matrix: %s[r%d][r%d]\n",
                   op.mat_label ? op.mat_label : "(null)",
                   op.val.matrix_regs[0], op.val.matrix_regs[1]);
            break;
        case ARG_NONE:
            printf("No operand\n");
            break;
        default:
            printf("Unknown operand type\n");
            break;
    }
}
/*for debug!*/

void print_opcode(ASTNode* node) {
    switch (node->opcode) {
        case OP_MOV:  printf("MOV\n"); break;
        case OP_CMP:  printf("CMP\n"); break;
        case OP_ADD:  printf("ADD\n"); break;
        case OP_SUB:  printf("SUB\n"); break;
        case OP_LEA:  printf("LEA\n"); break;
        case OP_CLR:  printf("CLR\n"); break;
        case OP_NOT:  printf("NOT\n"); break;
        case OP_INC:  printf("INC\n"); break;
        case OP_DEC:  printf("DEC\n"); break;
        case OP_JMP:  printf("JMP\n"); break;
        case OP_BNE:  printf("BNE\n"); break;
        case OP_JSR:  printf("JSR\n"); break;
        case OP_RED:  printf("RED\n"); break;
        case OP_PRN:  printf("PRN\n"); break;
        case OP_RTS:  printf("RTS\n"); break;
        case OP_STOP: printf("STOP\n"); break;
        default:      printf("Unknown OpCode\n"); break;
    }
}

void print_label(ASTNode* node) {
    if (node->label)
        printf("%s\n", node->label);
    else
        printf("(no label)\n");
}

void print_ast_node(ASTNode* node)
{
    if (!node) return;

    if (node->node_type == NODE_ERROR) {
        printf("Node Type: ERROR\n");
        if (node->error_info) {
            printf("Error Code: %d, Message: %s\n",
                   node->error_info->code, node->error_info->message);
        }
        if (node->original_line) {
            printf("Original line: %s\n", node->original_line);
        }
        return; 
    }

    if (node->node_type == NODE_DIRECTIVE) {
        printf("Node Type: DIRECTIVE\n");
        printf("Directive Type: %d\n", node->directive.dir_type);
        if (node->original_line) {
            printf("Original line: %s\n", node->original_line);
        }
        return; 
    }

    printf("OpCode: ");  print_opcode(node);
    printf("Operands:\n");
    print_operand(node->operands[0]);
    print_operand(node->operands[1]);
    printf("Label: ");   print_label(node);
    printf("Address: %d\n", node->address);
    printf("Original line: %s\n",
           node->original_line ? node->original_line : "(null)");
}



/*we use it in parser to figure which command we got */
OpCode opcode_from_text(const char* str) {
    if (strcmp(str, "mov") == 0)   return OP_MOV;
    if (strcmp(str, "cmp") == 0)   return OP_CMP;
    if (strcmp(str, "add") == 0)   return OP_ADD;
    if (strcmp(str, "sub") == 0)   return OP_SUB;
    if (strcmp(str, "lea") == 0)   return OP_LEA;
    if (strcmp(str, "clr") == 0)   return OP_CLR;
    if (strcmp(str, "not") == 0)   return OP_NOT;
    if (strcmp(str, "inc") == 0)   return OP_INC;
    if (strcmp(str, "dec") == 0)   return OP_DEC;
    if (strcmp(str, "red") == 0)   return OP_RED;
    if (strcmp(str, "prn") == 0)   return OP_PRN;
    if (strcmp(str, "jmp") == 0)   return OP_JMP;
    if (strcmp(str, "bne") == 0)   return OP_BNE;
    if (strcmp(str, "jsr") == 0)   return OP_JSR;
    if (strcmp(str, "rts") == 0)   return OP_RTS;
    if (strcmp(str, "stop") == 0)  return OP_STOP;
    return OP_NONE; 
}


/*  List debug printer     */


void print_ast_list(ASTNode *head)
{
    ASTNode *current = head;

    while (current != NULL)
    {
        printf("--------------------------------------------------\n");
        printf("Address: %d\n", current->address);

        if (current->label != NULL) {
            printf("Label: %s\n", current->label);
        }

        if (current->node_type == NODE_INSTRUCTION)
        {
            printf("Node Type: INSTRUCTION\n");
            print_opcode(current);

            printf("Operand 1: ");
            print_operand(current->operands[0]);
            printf("\n");

            printf("Operand 2: ");
            print_operand(current->operands[1]);
            printf("\n");
        }
        else if (current->node_type == NODE_DIRECTIVE)
        {
            printf("Node Type: DIRECTIVE\n");
            printf("Directive Type: %d\n", current->directive.dir_type);

            switch (current->directive.dir_type)
            {
            case DIR_DATA:
                printf(".data values: ");
                if (current->directive.value.data.count > 0)
                {
                    int i;
                    for (i = 0; i < current->directive.value.data.count; i++)
                        printf("%d ", current->directive.value.data.values[i]);
                }
                printf("\n");
                break;

            case DIR_STRING:
                printf(".string: \"%s\"\n",
                       current->directive.value.string_val ?
                       current->directive.value.string_val : "");
                break;

            case DIR_MAT:
            {
                int i, total;
                printf(".mat %dx%d: ",
                       current->directive.value.mat.rows,
                       current->directive.value.mat.cols);
                total = current->directive.value.mat.rows *
                        current->directive.value.mat.cols;
                for (i = 0; i < total; i++)
                    printf("%d ", current->directive.value.mat.values[i]);
                printf("\n");
                break;
            }

            case DIR_EXTERN:
                printf(".extern: %s\n",
                       current->directive.value.ext_label ?
                       current->directive.value.ext_label : "");
                break;

            case DIR_ENTRY:
                printf(".entry: %s\n",
                       current->directive.value.ext_label ?
                       current->directive.value.ext_label : "");
                break;

            default:
                printf("Unknown directive\n");
                break;
            }
        }
        else if (current->node_type == NODE_ERROR)
        {
            printf("Node Type: ERROR\n");
            if (current->error_info != NULL) {
                printf("Error Code: %d, Message: %s\n",
                       current->error_info->code,
                       current->error_info->message);
            }
        }

        if (current->original_line != NULL)
        {
            printf("Original line: %s\n", current->original_line);
        }

        current = current->next;
    }
    printf("--------------------------------------------------\n");
}