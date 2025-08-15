/*This is the parser.c, for more information go to parser.h*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "frontend_error.h"
#include "lexer.h"
#include "ast.h"
#define MAX_TOKEN_LINE 31
#define MAX_LINE_LEN 82
/*main functions*/
ASTNode* parser_parse_file(FILE* fp);
int parser_gather_tokens(const char* line, Token* tokens_arr, int max_tokens);
ASTNode* parser_parse_tokens(const Token* tokens, int num_tokens, int line_number, const char* line);
ASTNode* parser_parse_line(const char* line, int line_number);
void parser_free_ast( ASTNode* head);
static void sanitize_quotes(char *dst, size_t cap, const char *src);
/*help functions to analyze the file*/
void directive_type_from_text( ASTNode* node, Token* tokens_arr,int index);
static int is_directive_and_command(const Token* tokens_arr,int len);
static int is_command(const Token* tokens_arr,const  Token* tok,int index, int num_tokens);
static int next_nonsep(const Token* a, int i, int len);
static int is_immediate_token(const Token* a, int i, int len);
static int is_operand_start(const Token* a, int i, int len);
static int skip_operand(const Token* a, int i, int len);

 /* Builds a full AST for an assembly file.
 * Iterates over all lines and returns the head of the AST linked list.
 * Errors are collected in the error module and do not halt processing.*/
ASTNode* parser_parse_file(FILE* fp){
    char line[MAX_LINE_LEN];/*create a string to use in the fgets */
    ASTNode *head = NULL;
    ASTNode *tail = NULL, *node = NULL;/*for the linked list*/
    int line_number=1; /*counter*/
    while (fgets(line, MAX_LINE_LEN, fp)) {
        node = parser_parse_line(line, line_number);/*linked node, returns a pointer to head*/
        if (node == NULL) { line_number++; continue; }
        if (!head) { head = node; tail = node; }
        else { tail->next = node; tail = node; }
        line_number++;
    }
    return head;/*if everything went right we return the head of the linked list*/
}

/**
 * Converts a token array (representing a single line) to an ASTNode.
 * Checks syntax, detects errors, and fills all fields of the ASTNode.
 * param tokens_arr - the token array
 * param num_tokens - the number of tokens in the array
 * param line_number
 * param line - the string of the original line
 */
ASTNode* parser_parse_tokens(const Token* tokens_arr,
                             int num_tokens,            
                             int line_number,
                             const char* original_line) {

    int index = 0,is_entry, is_instruction, j;
    ASTNode* node = NULL; /*the node we return */
    char* label = NULL;/*we keep the starting label*/
    char* str = NULL;
    int values[256];
    int rows = 0, cols = 0, total = 0;/*for mat*/
    OpCode opcode = OP_NONE;/*if there is a command token*/
    Operand op1, op2;/*operand after command*/
    int count = 0;/*counter*/
    FrontErrorInfo err;/*for errors*/
    const char* holder_text;/*handeling the label*/
    op1 = empty_operand();/*initializing the operand*/
    op2 = empty_operand();/*initializing the operand*/
    /*in this section we are dealing with the Label*/
    if (num_tokens >= 2 &&tokens_arr[index].type == TOK_LABEL &&tokens_arr[index+1].type == TOK_SEPARATOR &&tokens_arr[index+1].text[0] == ':') {
        holder_text = tokens_arr[index].text;/*holder text of label*/
        if (holder_text!=NULL) {
            count = strlen(holder_text) + 1;
            label = (char*)malloc(count);
            if (label) memcpy(label, holder_text, count);
        } 
        else {label = NULL; }
        index += 2;
        while (index < num_tokens && tokens_arr[index].type == TOK_SEPARATOR && tokens_arr[index].text[0] != '"') {index++;}
    }
    count=0; holder_text=NULL;
    /*in the same sentance an operand and directive cannot be, if there are we report to err*/
    if (is_directive_and_command(tokens_arr + index, num_tokens - index)) {
        err.code = ERR_SYNTAX;
        err.line = line_number;
        strncpy(err.message, "Directive and command cannot be in the same line", MAX_ERROR_MSG-1);
        err.message[MAX_ERROR_MSG-1] = '\0';
        if (label) free(label);
        return build_error_node(&err, original_line);
    }
    /*In case this is a sentance with directive*/
    if (index < num_tokens && tokens_arr[index].type == TOK_DIRECTIVE) {
        if (strcmp(tokens_arr[index].text, ".data") == 0) { /*the directive data*/
            index++;/*we continue to the next token*/
            while (index < num_tokens && tokens_arr[index].type != TOK_EOF) {/*we are making sure that after .data there are only numbers*/
                if (tokens_arr[index].type == TOK_NUMBER) {
                    values[count++] = atoi(tokens_arr[index].text);
                    index++;
                    if (index < num_tokens && tokens_arr[index].type == TOK_SEPARATOR && strcmp(tokens_arr[index].text, ",") == 0)
                        index++;
                } else {
                    break;
                }
            }
            node = build_directive_node(DIR_DATA, label, values, count, 0, original_line);/*if everything is right we bulit AST node*/
        } 
        else if (strcmp(tokens_arr[index].text, ".string") == 0) {/*the directive string*/
            index++;
            if (index < num_tokens && tokens_arr[index].type == TOK_STRING) {/*we are making sure that after .string there are only string*/
                holder_text = tokens_arr[index].text;
                node = build_directive_node(DIR_STRING, label, holder_text, 0, 0, original_line);/*if everything is right we bulit AST node*/
            }      
            else {/*if something falied we built Ast error, so the next pat will know*/
                FrontErrorInfo err;
                err.code = ERR_SYNTAX;
                err.line = line_number;
                strncpy(err.message, ".string expects a string", MAX_ERROR_MSG - 1);
                err.message[MAX_ERROR_MSG - 1] = '\0';
                if (label) free(label);
                return build_error_node(&err, original_line);
            }
        } 
        else if (strcmp(tokens_arr[index].text, ".mat") == 0) {/*the directive mat*/
            holder_text=NULL;
            index++;/*we are making sure that after .mat there are only tokens in mat format*/
            if (index+5 < num_tokens && tokens_arr[index].type   == TOK_SEPARATOR && tokens_arr[index].text[0] == '[' && tokens_arr[index+1].type == TOK_NUMBER && tokens_arr[index+2].type == TOK_SEPARATOR && tokens_arr[index+2].text[0] == ']' && tokens_arr[index+3].type == TOK_SEPARATOR && tokens_arr[index+3].text[0] == '[' && tokens_arr[index+4].type == TOK_NUMBER && tokens_arr[index+5].type == TOK_SEPARATOR && tokens_arr[index+5].text[0] == ']') {
                rows = atoi(tokens_arr[index+1].text);
                cols = atoi(tokens_arr[index+4].text);
                index += 6;/*everything must be in a directive mat format so we make sure the tokens are fitting */
                total = rows * cols;
                while (index < num_tokens && count < total) {/*usually after a directive mat there needs to be numbers*/
                    if (tokens_arr[index].type == TOK_NUMBER) {
                        values[count++] = atoi(tokens_arr[index].text);
                        index++;
                        if (index < num_tokens && tokens_arr[index].type == TOK_SEPARATOR && strcmp(tokens_arr[index].text, ",") == 0)
                            index++;
                    } else {
                        break;
                    }
                }
                node = build_directive_node(DIR_MAT, label, values, rows, cols, original_line);/*if everything is right we bulit AST node*/
            } 
            else {/*if something falied we built Ast error, so the next pat will know*/
                err.code = ERR_SYNTAX;
                err.line = line_number;
                strncpy(err.message, ".mat expects [rows][cols] numbers", MAX_ERROR_MSG-1);
                err.message[MAX_ERROR_MSG-1] = '\0';
                if (label) free(label);
                return build_error_node(&err, original_line);
            }
        } 
        else if (strcmp(tokens_arr[index].text, ".entry") == 0 || strcmp(tokens_arr[index].text, ".extern") == 0) {
             count=0;
            holder_text=NULL;
            is_entry = (tokens_arr[index].text[1] == 'e'); /*the directive entry*/
            index++;
            if (index < num_tokens && tokens_arr[index].type == TOK_LABEL) {/*after an entry comes an inner label*/
                holder_text = tokens_arr[index].text;
                if (holder_text!=NULL) {
                    count = strlen(holder_text) + 1;
                    str = (char*)malloc(count);
                    if (str) memcpy(str, holder_text, count);
                } else {
                    str = NULL;
                }
                node = build_directive_node(is_entry ? DIR_ENTRY : DIR_EXTERN, label, str, 0, 0, original_line);/*if everything is right we bulit AST node*/
            } else {
                err.code = ERR_SYNTAX;
                err.line = line_number;
                strncpy(err.message, ".entry/.extern expects a label", MAX_ERROR_MSG-1);
                err.message[MAX_ERROR_MSG-1] = '\0';
                if (label) free(label);
                return build_error_node(&err, original_line);
            }
        } else {/*if something falied we built Ast error, so the next pat will know*/
            err.code = ERR_SYNTAX;
            err.line = line_number;
            strncpy(err.message, "Unknown directive", MAX_ERROR_MSG-1);
            err.message[MAX_ERROR_MSG-1] = '\0';
            if (label) free(label);
            return build_error_node(&err, original_line);
        }

        if (label) free(label);/*we are making sure to free all memory of this secton*/
        if (str) free(str);
        return node;
    }
    count=0;
    /* In case of an instruction(I also called it mnomics or commands so you know its the same)*/
    if (index < num_tokens && tokens_arr[index].type == TOK_MNEMONIC && is_command(tokens_arr, &tokens_arr[index], index, num_tokens) != -1) {
        opcode = opcode_from_text(tokens_arr[index].text);
        j = index + 1;/*for comfort*/
        /*we skip separator before the first operand*/
        while (j < num_tokens && tokens_arr[j].type == TOK_SEPARATOR && tokens_arr[j].text[0] != '"') j++;
        is_instruction = is_command(tokens_arr, &tokens_arr[index], index, num_tokens);
        if (is_instruction == 2) {
            op1 = build_operand_full(tokens_arr, &j, num_tokens);
            while (j < num_tokens && tokens_arr[j].type == TOK_SEPARATOR && strcmp(tokens_arr[j].text, ",") == 0) j++;/*we skip separator before the second operand*/
            op2 = build_operand_full(tokens_arr, &j, num_tokens);
        } else if (is_instruction == 1) {
            op1 = build_operand_full(tokens_arr, &j, num_tokens);
            op2 = empty_operand();
        } else {
            op1 = empty_operand();
            op2 = empty_operand();
        }
        /*usinig malloc we build handly the node*/
        node = (ASTNode*)malloc(sizeof(ASTNode));
        if (!node) {
            if (label) free(label);
            return NULL;
        }/*and fill al the details, shorter then with directives*/
        node->node_type = NODE_INSTRUCTION;
        node->opcode = opcode;
        node->operands[0] = op1;
        node->operands[1] = op2;
        node->label = label; /*we delt with it in the begining*/
        node->address = 0;
        /*if everything is right we bulit AST node*/
        if (original_line) {
            count = strlen(original_line) + 1;
            node->original_line = (char*)malloc(count);
            if (node->original_line) memcpy(node->original_line, original_line, count);
        } else {
            node->original_line = NULL;
        }

        node->next = NULL;
        node->error_info = NULL;
        node->directive.dir_type = DIR_NONE;
        
        return node;
    }
    /*if something falied we built Ast error, so the next pat will know*/
    if (label) free(label);
    err.code = ERR_SYNTAX;
    err.line = line_number;
    strncpy(err.message, "Couldn't build node", MAX_ERROR_MSG-1);
    err.message[MAX_ERROR_MSG-1] = '\0';
    return build_error_node(&err, original_line);
}

/* builds an ASTNode for a single line (calls both gather and parse). */
ASTNode* parser_parse_line(const char* line, int line_number){
    int tokenNum=0; /*counter*/
    int only_seps = 1;
    int k;
    Token tokens_arr[MAX_TOKEN_LINE];/*the token's array*/
    FrontErrorInfo err;/*the error reporter*/
    tokenNum=parser_gather_tokens(line,tokens_arr,MAX_TOKEN_LINE);/*we use the function to gather all the tokens in a line */
    if (tokenNum <= 0) return NULL;
    for (k = 0; k < tokenNum; ++k) {
            if (tokens_arr[k].type != TOK_SEPARATOR) {
                only_seps = 0;
                break;
            }
        }
    if (tokenNum == 0 || only_seps) {
        return NULL;
    }
    if(tokenNum==-1){ /*if something falied we built Ast error, so the next pat will know*/
        err.code = ERR_INVALID_OPERAND;
        err.line = line_number;
        strncpy(err.message, "Check your operands", MAX_ERROR_MSG-1);
        err.message[MAX_ERROR_MSG-1] = '\0';
        return build_error_node(&err, line);
    }
    if(tokenNum==0){
        return NULL;
    }
    return parser_parse_tokens(tokens_arr,tokenNum, line_number,line);/*we use the function and return the ast node*/
}

/* Collects all tokens from a single line using the lexer. */
int parser_gather_tokens(const char* line, Token* tokens_arr, int max_tokens){
    int countTokens=0;/*counters*/
    Token* tok;/*the token we get in every part of the loop*/
    char str_line[MAX_LINE_LEN];
    sanitize_quotes(str_line, sizeof(str_line), line);
    lexer_init_line(str_line);  /*initialize the details in lexer.c*/
    tok = lexer_next_token();/*we use the function in lexer.c to get a token */
    while (tok != NULL && tok->type != TOK_EOF) {/*we want to gather all the tokens in a line to a an array*/
        if (countTokens >= max_tokens) {/*we check the limit*/
            printf("not in limit");
            return -1;
        }
        tokens_arr[countTokens++] = *tok; /*we put in the array*/
        lexer_free_token(tok);            /* free memory*/
        tok = lexer_next_token();
    }
    if (tok) lexer_free_token(tok); /* free memory*/
    if (countTokens < max_tokens) {
        tokens_arr[countTokens].type = TOK_EOF;/*we making sure that the array always ends in eof tok*/
        tokens_arr[countTokens].text[0] = '\0';
        countTokens++;
    }
    return countTokens;
}
/*The function checks if the toke is a type of directive.if the token is a type of directive return we declare it as such. */
void directive_type_from_text(ASTNode* node,Token* tokens_arr,int index){
    if (strcmp(tokens_arr[index].text, ".data") == 0)
        node->directive.dir_type = DIR_DATA;
    else if (strcmp(tokens_arr[index].text, ".string") == 0)
        node->directive.dir_type = DIR_STRING;
    else if (strcmp(tokens_arr[index].text, ".mat") == 0)
        node->directive.dir_type = DIR_MAT;
    else if (strcmp(tokens_arr[index].text, ".extern") == 0)
        node->directive.dir_type = DIR_EXTERN;
    else if (strcmp(tokens_arr[index].text, ".entry") == 0)
        node->directive.dir_type = DIR_ENTRY;
}

/*The function checks that in a line there arn't both directive and command*/
static int is_directive_and_command(const Token* tokens_arr,int len){
    int i,dir=0, cmd=0;
    for (i = 0; i < len; ++i) {
        if (tokens_arr[i].type == TOK_DIRECTIVE)dir = 1;
        if (tokens_arr[i].type == TOK_MNEMONIC)cmd = 1;
    }
    if(dir==1 && cmd==1) return 1;
    return 0;
}
/*This fuction not only checks if a token is command, it also checks 
* that every command has the right amount of operands.
* we return the number of operands.
*/
static int is_command(const Token* tokens_arr,const  Token* tok,int index, int num_tokens){
    static const char* const cmd[] = {
        "mov","cmp","add","sub","lea","clr","not","inc","dec","red",
        "prn","jmp","bne","jsr","rts","stop"
    };
    int which = -1, i;
    int p1, after_p1,j,  p2;
    int src,dest;
    /* type of command*/
    for (i = 0; i < 16; i++) {
        if (strcmp(cmd[i], tok->text) == 0) {
            which = i; 
            break; }
    }
    if (which == -1) return -1;

    /* first operand and its location*/
    p1 = next_nonsep(tokens_arr, index+1, num_tokens);
    after_p1 = skip_operand(tokens_arr, p1, num_tokens);

    /*mov/cmp/add/sub/lea  */
    if (which==0 || which==1 || which==2 || which==3 || which==4) {
        /* there must be separator betwen two operands*/
        if (after_p1 >= num_tokens) return -1;
        if (tokens_arr[after_p1].type != TOK_SEPARATOR || tokens_arr[after_p1].text[0] != ',')return -1;
        /* second operand*/
        p2 = next_nonsep(tokens_arr, after_p1 + 1, num_tokens);

        /* both operand must be legal*/
        if (!is_operand_start(tokens_arr, p1, num_tokens)) return -1;
        if (!is_operand_start(tokens_arr, p2, num_tokens)) return -1;

        /*those command cannot have immediate as second operand */
        if (which==0 || which==2 || which==3) {
            if (is_immediate_token(tokens_arr, p2, num_tokens)) return -1;
        }

        /* LEA: src=LABEL/MATRIX, dst=REGISTER/MATRIX */
        if (which==4) {
            src = (tokens_arr[p1].type==TOK_LABEL || tokens_arr[p1].type==TOK_MATRIX_OP);
            dest = (tokens_arr[p2].type==TOK_REGISTER || tokens_arr[p2].type==TOK_MATRIX_OP);
            if (!src || !dest) return -1;
        }
        return 2;
    }
    j=-1;
    /*  clr/not/inc/dec/red/prn  */
    if (which>=5 && which<=10) {
        if (!is_operand_start(tokens_arr, p1, num_tokens)) return -1;
        /*making sure that arn's operand is legat */
        if (which!=10 && is_immediate_token(tokens_arr, p1, num_tokens)) return -1;
        return 1;
    }

    /*jmp/bne/jsr  */
    if (which>=11 && which<=13) {
        if (!(tokens_arr[p1].type==TOK_LABEL || tokens_arr[p1].type==TOK_MATRIX_OP)) return -1;
        return 1;
    }

    /*  rts/stop are without operands */
    if (which==14 || which==15) {
        j = next_nonsep(tokens_arr, after_p1, num_tokens);
        /* אם נשאר משהו שהוא אופרנד אמיתי -> שגיאה */
        if (j < num_tokens && is_operand_start(tokens_arr, j, num_tokens)) return -1;
        return 0;
    }

    return -1;

}
/*we want to free memory in the ast node*/
void parser_free_ast( ASTNode* head) {
    ASTNode* curr = head;
    ASTNode* next;
    while (curr) {
        next = curr->next;
        free(curr);
        curr = next;
    }
}

/*a helping function that skips separator*/
static int next_nonsep(const Token* a, int i, int len)
{
    while (i < len && a[i].type == TOK_SEPARATOR) { i++;}
    return i;
}
/*this helper function checks if the operand is a register #*/
static int is_immediate_token(const Token* a, int i, int len)
{
    if (i < len && a[i].type == TOK_NUMBER && a[i].text[0] == '#') {
        return 1;
    }
    if (i+1 < len && a[i].text[0] == '#' && a[i+1].type == TOK_NUMBER) {
        return 1;
    }
    return 0;
}
/*this helper function checks if the operand can be a source operand*/
static int is_operand_start(const Token* a, int i, int len)
{
    if (i >= len) return 0;
    if (a[i].type == TOK_LABEL)      return 1;
    if (a[i].type == TOK_REGISTER)   return 1;
    if (a[i].type == TOK_MATRIX_OP)  return 1;
    if (is_immediate_token(a, i, len)) return 1;
    return 0;
}
/*this function goal is to return the index of the first token after the first operand token */
static int skip_operand(const Token* a, int i, int len)
{
    int j;
    if (i >= len) return i;
    /* if the first operand in immidate we skip to the next operand */
    if (a[i].type == TOK_NUMBER && a[i].text[0] == '#') {
        return i + 1;
    }
    if (a[i].text[0] == '#' && i + 1 < len && a[i + 1].type == TOK_NUMBER) {
        return i + 2;
    }
    /* if the first operand is mat or register we skip to the next one*/
    if (a[i].type == TOK_REGISTER || a[i].type == TOK_MATRIX_OP) {
        return i + 1;
    }
    /* checking label and mat */
    if (a[i].type == TOK_LABEL) {
        j = i + 1;
        if (j + 5 < len &&
            a[j].type     == TOK_SEPARATOR && a[j].text[0]     == '[' &&
            a[j+1].type   == TOK_REGISTER &&
            a[j+2].type   == TOK_SEPARATOR && a[j+2].text[0]   == ']' &&
            a[j+3].type   == TOK_SEPARATOR && a[j+3].text[0]   == '[' &&
            a[j+4].type   == TOK_REGISTER &&
            a[j+5].type   == TOK_SEPARATOR && a[j+5].text[0]   == ']')
        {
            /* skip the mat*/
            return j + 6;
        }
        /* simple label*/
        return i + 1;
    }
    return i + 1;
}
/*scan the string character by character, detect the UTF-8 byte sequence for curly quotes (“ or ”), and replace it with a regular double quote (").”*/
static void sanitize_quotes(char *dst, size_t cap, const char *src){
    size_t i = 0, j = 0;
    if (!dst || !src || cap == 0) return;

    while (src[i] != '\0' && j + 1 < cap) {
        unsigned char c0 = (unsigned char)src[i];
        if (c0 == 0xE2 &&(unsigned char)src[i+1] == 0x80 && ((unsigned char)src[i+2] == 0x9C || (unsigned char)src[i+2] == 0x9D)) {
            dst[j++] = '"';
            i += 3;
            continue;
        }
        dst[j++] = src[i++];
    }
    dst[j] = '\0';
}
/*we make sure that the mat is habdeled and not broken
static int broken_mat(const Token* a, int i, int len) {
    return (i+1 < len &&
            a[i].type == TOK_LABEL &&
            a[i+1].type == TOK_SEPARATOR && a[i+1].text[0] == ']');
}
            */