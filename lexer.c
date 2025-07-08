#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  <ctype.h>
#include "frontend_error.h"
#include "lexer.h"

/* משתנים פנימיים */
static const char *cur_line;
static size_t      cur_pos;
static int         cur_line_no;  /* יעדכן parser_parse_file */

void lexer_init_line(const char *line) {
    cur_line = line;   /* שמור מצביע לשורה שמפורקת */
    cur_pos  = 0;      /* האתחול מתחיל מהתו הראשון */
    /* שימי לב: cur_line_no צריך להתעדכן חיצונית לפני הקריאה */
}

Token* lexer_next_token(void){
   



}





void   lexer_free_token(Token *tok);

/** דילוג על ריווח והערות */
static void skip_whitespace_and_comments(void){
     while (cur_line[cur_pos]){
        if(cur_line[cur_pos] == ' ' || cur_line[cur_pos] == '\r' || cur_line[cur_pos]== '\n' || cur_line[cur_pos]== '\t')
            cur_pos++;     
     }
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