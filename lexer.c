#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  <ctype.h>
#include "frontend_error.h"
#include "lexer.h"


static const char *cur_line;
static size_t      cur_pos;
static int         cur_line_no;  /* יעדכן parser_parse_file */
static Token* make_token(TokenType type, size_t start, size_t length);
void lexer_init_line(const char *line) {
    cur_line = line;   /* שמור מצביע לשורה שמפורקת */
    cur_pos  = 0;      /* האתחול מתחיל מהתו הראשון */
    /* שימי לב: cur_line_no צריך להתעדכן חיצונית לפני הקריאה */
}

Token* lexer_next_token(void){
    size_t start, length;
    char token_text[MAX_TOKEN_LEN];
    while (cur_line[cur_pos] == ' ' || cur_line[cur_pos]=='\t')
    {
        cur_pos++;
    }
    
    if(cur_line[cur_pos] == '\0'){
        return make_token(TOK_EOF, cur_pos,0);
    }

     if(cur_line[cur_pos] == ';'){
        return make_token(TOK_EOF, cur_pos,0);
    }

    if(cur_line[cur_pos] == ':' || cur_line[cur_pos] == ','  || cur_line[cur_pos] == '[' || cur_line[cur_pos] == ']' || cur_line[cur_pos] == '(' || cur_line[cur_pos] == ')'){
        start = cur_pos;
        cur_pos++;
        return make_token(TOK_SEPARATOR,start, 1);
    }

    if(cur_line[cur_pos] == '"'){
        start = cur_pos;
        cur_pos++;
        while(cur_line[cur_pos] != '"' && cur_line[cur_pos] != '\0'){
            cur_pos++;
        }
        if (cur_line[cur_pos] != '"') {
            return make_token(TOK_INVALID,start, cur_pos-start);
        }
        else{
            cur_pos++; 
            return make_token(TOK_DIRECTIVE, start, cur_pos-start);
        }
    }

    if (isalpha(cur_line[cur_pos]) || cur_line[cur_pos] == '.' ||
        cur_line[cur_pos] == '#' || cur_line[cur_pos] == '-' ||
        isdigit(cur_line[cur_pos])) {

        start = cur_pos;
        while (isalnum(cur_line[cur_pos]) || cur_line[cur_pos] == '_' || cur_line[cur_pos] == ':' || cur_line[cur_pos] == '.' || cur_line[cur_pos] == '#') {
            cur_pos++;
        }
        length = cur_pos - start;
        if (length >= MAX_TOKEN_LEN)
            length = MAX_TOKEN_LEN - 1;
        memcpy(token_text, cur_line + start, length);
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
        } else if (is_matrix(token_text)) { 
            return make_token(TOK_MATRIX, start, length);
        } else {
            /* אם זה לא מזוהה — אולי תוסיפי סוג אחר */
            return  make_token(TOK_INVALID, start, length);
        }
    }
    start = cur_pos;
    cur_pos++;
    return make_token(TOK_SEPARATOR, start, 1);
}


int is_label(const char *token_text) {
    int end = (int)strlen(token_text) - 1;
    if(end>0 && token_text[end] == ':' && token_text[end - 1] != ' ' && !is_mnemonic(token_text) && !is_register(token_text) && valid_len(token_text, 31))
        return 1;
    return 0;
}
int is_register(const char *token_text){
    if(strlen(token_text)==2 && token_text[0]=='r' && token_text[1]>='0' && token_text[1]<='7'){
        return 1;
    }
    return 0;
}
int is_mnemonic(const char *token_text){
    const char* mnemonics[] = { "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "red",
    "prn", "jmp", "bne", "jsr", "rts", "stop"};
    int i=0;
    for(i;i<sizeof(mnemonics)/ sizeof(mnemonics[0]);i++){
        if(strcmp(token_text,mnemonics[i])==0){
            return 1;
        }
    }
    return 0;
}
int is_number(const char *token_text){
    int i=1,j=0;
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

int is_directive(const char *token_text){
    const char *directives[] = {".data", ".string", ".extern", ".entry"};
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

void lexer_free_token(Token *tok) {
    free(tok);
}

int is_matrix(const char *token_text) {
    size_t temp_pos = cur_pos;
    while (cur_line[temp_pos] == ' ' || cur_line[temp_pos] == '\t')
        temp_pos++;
    if (cur_line[temp_pos] == '[') {
        if (!is_label(token_text) && !is_register(token_text) && !is_mnemonic(token_text)
            && !is_number(token_text) && !is_directive(token_text)) {
            return 1;
        }
    }
    return 0;
}


/** מקצה Token, מעתיק טקסט, ומחזיר מצביע */
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

int valid_len(const char *token_text, int num) {
    return (strlen(token_text) < num);
}