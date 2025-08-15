 /*  Input:  Each line of assembly code as a string.
 * Output: Pointer to Token struct representing the next token in the line. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "frontend_error.h"
#include "lexer.h"

/*
 * Holds the current line for lexing, current position in the line,
 * and current line number (should be set from outside).
 */
static const char *cur_line;
static size_t      cur_pos;
static int         cur_line_no;/* Should be updated by parser_parse_file */

static Token* make_token(TokenType type, size_t start, size_t length);

/*this function initialize all the details*/
void lexer_init_line(const char *line) {
    cur_line = line;  /* Save pointer to the input line */
    cur_pos  = 0;    /* Start at first character */
 
}
/*the main function in this file, this function 'cut' a part by index to proper token.*/
Token* lexer_next_token(void) {
    size_t start, length;/*imitialize to cut by index*/
    char token_text[MAX_TOKEN_LEN];/*token text*/
    char name[MAX_TOKEN_LEN], row_reg[8], col_reg[8];/*for mat*/
    while (cur_line[cur_pos] == ' ' || cur_line[cur_pos] == '\t') {/*we skip tabs*/
        cur_pos++;
    }
    if (cur_line[cur_pos] == '\0') {/*in case we are in the end of the line*/
        return make_token(TOK_EOF, cur_pos, 0);
    }
    if (cur_line[cur_pos] == ';') {/*in case of comment*/
        return make_token(TOK_EOF, cur_pos, 0);
    }
    /*in case of separtors, we cut them to different tokens*/
    if (cur_line[cur_pos] == ':' || cur_line[cur_pos] == ',' ||cur_line[cur_pos] == '[' || cur_line[cur_pos] == ']' ||cur_line[cur_pos] == '(' || cur_line[cur_pos] == ')') {
        start = cur_pos;
        cur_pos++;
        return make_token(TOK_SEPARATOR, start, 1);
    }
    if ((unsigned char)cur_line[cur_pos] == 0xE2) {/*we use bits to handle different cases of closures*/
        size_t start = cur_pos;
        cur_pos++;                      /* prevent unlimitid loops*/
        return make_token(TOK_INVALID, start, 1);
    }
    if (cur_line[cur_pos] == '"') {/*in case of a string*/
        start = cur_pos;
        cur_pos++; /* after "*/
        while (cur_line[cur_pos] != '"' && cur_line[cur_pos] != '\0') {/*we want to add all the string */
            cur_pos++;
        }
        if (cur_line[cur_pos] != '"') {/*closing the string */
            return make_token(TOK_INVALID, start, cur_pos - start);
        }
        cur_pos++; /* including the '' */
        return make_token(TOK_STRING, start, cur_pos - start);
    }
    /*in case of any type of numbers*/
    if (cur_line[cur_pos] == '-' && isdigit(cur_line[cur_pos + 1])) {
        start = cur_pos++;
        while (isdigit(cur_line[cur_pos])) cur_pos++;
        return make_token(TOK_NUMBER, start, cur_pos - start);
    }
    if (isdigit(cur_line[cur_pos])) {
        start = cur_pos;
        while (isdigit(cur_line[cur_pos])) cur_pos++;
        return make_token(TOK_NUMBER, start, cur_pos - start);
    }
    /*this long section deals with registers,instructions,number or directive*/
    if (isalpha(cur_line[cur_pos]) || cur_line[cur_pos] == '.' ||cur_line[cur_pos] == '#'|| isdigit(cur_line[cur_pos])) {/*to identify*/
        start = cur_pos;
        while (isalnum(cur_line[cur_pos]) || cur_line[cur_pos] == '_' ||
               cur_line[cur_pos] == '.' || cur_line[cur_pos] == '#') {
            cur_pos++;
        }
        length = cur_pos - start;
        if (length >= MAX_TOKEN_LEN)/*checking limit*/
            length = MAX_TOKEN_LEN - 1;
        memcpy(token_text, cur_line + start, length);/*to copy the string*/
        token_text[length] = '\0';

        if (is_label(token_text)) {
            return make_token(TOK_LABEL, start, length);
        } else if (is_register(token_text)) {
            return make_token(TOK_REGISTER, start, length);
        } else if (is_mnemonic(token_text)) {
            return make_token(TOK_MNEMONIC, start, length);
        } else if (is_number(token_text)) {
            return make_token(TOK_NUMBER, start, length);
        } else if (is_directive(token_text)) {
            return make_token(TOK_DIRECTIVE, start, length);
        }
        /* mat case*/
        if (is_matrix_operand(token_text, name, row_reg, col_reg)) {
            Token* t = make_token(TOK_MATRIX_OP, start, length);
            strcpy(t->matrix_name, name);
            strcpy(t->row_register, row_reg);
            strcpy(t->col_register, col_reg);
            return t;
        } else if (is_matrix_dim(token_text)) {
            Token* t = make_token(TOK_MATRIX, start, length);
            t->matrix_dim = 1;
            return t;
        } else {
            return make_token(TOK_INVALID, start, length);
        }
    }
    start = cur_pos;
    cur_pos++;
    return make_token(TOK_SEPARATOR, start, 1);/*we return the built token*/
}

/*
 * Checks if the token text is a valid label.
 * Returns 1 if yes, 0 otherwise.
 */
int is_label(const char *token_text) {
    int i, len;
    if (!token_text)return 0;
    len = strlen(token_text);
    if (len < 1 || len > 31)
        return 0;
    if (token_text[0] < 'A' || token_text[0] > 'Z')/*label must be capital letters*/
        return 0;
    if (is_mnemonic(token_text) || is_register(token_text) || is_directive(token_text))/*label cannot be something else*/
        return 0;
    for (i = 1; i < len; i++) {
        if (!(token_text[i] >= 'A' && token_text[i] <= 'Z') &&
            !(token_text[i] >= '0' && token_text[i] <= '9')) {
            return 0;
        }
    }
    for (i = 0; i < len; i++) {
        if (token_text[i] < '0' || token_text[i] > '9')
            return 1; 
    }
    return 0; 
}

/*
 * Checks if the token is a valid register name r0-r7.
 * Returns 1 if valid, 0 otherwise.
 */
int is_register(const char *token_text){
    if(strlen(token_text)==2 && token_text[0]=='r' && token_text[1]>='0' && token_text[1]<='7'){
        return 1;
    }
    return 0;
}

/*
 * Checks if the token is a valid mnemonic.
 * Returns 1 if valid mnemonic, 0 otherwise.
 */
int is_mnemonic(const char *token_text){
    const char* mnemonics[] = { "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "red",
    "prn", "jmp", "bne", "jsr", "rts", "stop"};
    size_t i,n;
    n = sizeof(mnemonics) / sizeof(mnemonics[0]);
    for(i = 0; i < n; i++){
        if(strcmp(token_text,mnemonics[i])==0){
            return 1;
        }
    }
    return 0;
}

/*
 * Checks if the token is a number of register, starting with '#' and then digits. can be positive or negative
 * Returns 1 if number, 0 otherwise.
 */
int is_number(const char *token_text){
    int i=1;
    int len = strlen(token_text);
    if (len < 2) return 0;
    if (token_text[0] != '#') return 0;
    if (token_text[i] == '+' || token_text[i] == '-') i++;
    if (!isdigit(token_text[i])) return 0; 

    for (; i < len; i++) {
        if (!isdigit(token_text[i]))
            return 0;
    }
    return 1;
}

/*
 * Checks if the token is a directive (e.g., .data, .string).
 * Returns 1 if directive, 0 otherwise.
 */
int is_directive(const char *token_text){
    const char *directives[] = {".data", ".mat", ".string", ".extern", ".entry"};
    int num_directives = sizeof(directives) / sizeof(directives[0]);
    int i;
    if (token_text == NULL || token_text[0] != '.')
        return 0; 
    for (i = 0; i < num_directives; i++) {
        if (strcmp(token_text, directives[i]) == 0)
            return 1; 
    }
    return 0; 
}

/* Frees the memory for a Token object. */
void lexer_free_token(Token *tok) {
    free(tok);
}

/*
 * Checks if the token is a matrix name, followed by '[' in the line.
 * Only returns true if it's not recognized as another type.
 * Returns 1 if matrix, 0 otherwise.
 */
int is_matrix_dim(const char *token_text) {
    int num = 0, len, i;
    len = (int)strlen(token_text);
    if (len < 3) return 0;
    if (token_text[0] != '[' || token_text[len-1] != ']') return 0;
    for (i = 1; i < len-1; ++i) {
        if (i == 1 && token_text[i] == '-') continue;
        if (!isdigit(token_text[i])) return 0;
    }
    if (sscanf(token_text, "[%d]", &num) == 1) {
        return 1;
    }
    return 0;
}

/*
 * Internal: Allocates a new Token, copies the text, sets type/line.
 * Handles memory allocation error with front_error_report.
 * Returns pointer to Token or NULL on failure.
 */
static Token* make_token(TokenType type, size_t start, size_t length){
    Token *tok = malloc(sizeof *tok);
    if (!tok) {
        FrontErrorInfo err;
        err.code = ERR_OUT_OF_MEMORY;
        err.line = cur_line_no;
        strncpy(err.message, "malloc failed in make_token", MAX_ERROR_MSG - 1);
        err.message[MAX_ERROR_MSG - 1] = '\0';
        front_error_report(&err);
        return NULL;
    }
    memcpy(tok->text, cur_line + start, length);
    tok->text[length] = '\0';
    tok->type = type;
    tok->line = cur_line_no;
    return tok;
}

/*this function checks all the case of a token to be the type of mat that comes as an operand*/
int is_matrix_operand(const char *token_text, char *name_out, char *row_reg_out, char *col_reg_out)
{
    const char *p1,*p2,*p3,*p4;/*pointers*/
    size_t name_len, row_len, col_len;
    char temp_name[MAX_TOKEN_LEN];
    char temp_row[8];
    char temp_col[8];
    /*searching for first []*/
    p1 = strchr(token_text, '[');
    if (p1 == NULL) return 0;
    p2 = strchr(p1, ']');
    if (p2 == NULL)return 0;
    p3 = strchr(p2, '[');
    if (p3 == NULL)return 0;
    p4 = strchr(p3, ']');
    if (p4 == NULL)return 0;
    if (*(p4 + 1) != '\0')
        return 0; /* after an operand mat there is nothing*/

    /* checking the mat name len*/
    name_len = (size_t)(p1 - token_text);
    if (name_len < 1 || name_len > 30)
        return 0;
    strncpy(temp_name, token_text, name_len);
    temp_name[name_len] = '\0';

    /* the immidiate between first []*/
    row_len = (size_t)(p2 - (p1 + 1));
    if (row_len != 2)
        return 0;

    strncpy(temp_row, p1 + 1, row_len);
    temp_row[row_len] = '\0';

    /* the immidiate between second []*/
    col_len = (size_t)(p4 - (p3 + 1));
    if (col_len != 2)
        return 0;
    strncpy(temp_col, p3 + 1, col_len);
    temp_col[col_len] = '\0';
    if (!is_label(temp_name))
        return 0;
    if (!is_register(temp_row))
        return 0;
    if (!is_register(temp_col))
        return 0;
    strcpy(name_out, temp_name);
    strcpy(row_reg_out, temp_row);
    strcpy(col_reg_out, temp_col);
    return 1;
}
/*lior no change */