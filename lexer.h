/*this is the lexer file
*This part of the project is another step to be able to analyze text and make it an AST linked list.
The lexer goal is to separate a line into tokens that represent the label,regster,number, command ect....
The parser uses the lexer and is the one to cut the line by location to different token
The lexer job is to figure the type of token for every part of the line*/
#ifndef LEXER_H
#define LEXER_H
#define MAX_TOKEN_LEN 64
#include "frontend_error.h"  
#include <stddef.h>        

typedef enum {
    TOK_LABEL,      
    TOK_MNEMONIC,   /*< commands */
    TOK_REGISTER,   /*<register*/
    TOK_NUMBER,     /*< number */
    TOK_DIRECTIVE,  /*< directive */
    TOK_STRING,   /*< string */
    TOK_SEPARATOR,  /**separators like , */
    TOK_EOF,         /*end of line/file/;*/
    TOK_MATRIX,     /*mat*/
    TOK_MATRIX_OP,/*operand mat*/
    TOK_OTHER, /*after entry or extreme*/
    TOK_INVALID
} TokenType;


typedef struct {
    TokenType type;               /*type of token*/
    char      text[MAX_TOKEN_LEN];/*the exact text of token*/
    int       line;               /*number of line*/
    char      matrix_name[MAX_TOKEN_LEN]; /*name of mat if there is */
    char      row_register[8];           /*register for mat*/
    char      col_register[8];           /*register for mat*/
    int       matrix_dim;                /*for mat*/
} Token;

/**
 * Initialize the lexer to a new line,inculding the location in the line
 */
void   lexer_init_line(const char *line);

/**
 * the main function, separate the line into tokens and return the next the token in line
 */
Token* lexer_next_token(void);

void   lexer_free_token(Token *tok);

/*helper function to help us analyze the line and tokens*/
int is_label(const char *token_text);
int is_register(const char *token_text);
int is_mnemonic(const char *token_text);
int is_number(const char *token_text);
int is_directive(const char *token_text);
int is_matrix_dim(const char *token_text);
int valid_len(const char *token_text, int num);
int is_matrix_operand(const char* token_text, char* name, char* row_reg, char* col_reg);
#endif /* LEXER_H */