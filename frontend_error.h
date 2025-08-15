/*This file is helping us to report errors in the text and project*/
#ifndef FRONTEND_ERROR_H
#define FRONTEND_ERROR_H

#define MAX_ERROR_MSG 128 

typedef enum {
    ERR_UNEXPECTED_TOKEN,   /* unexpected token*/
    ERR_LABEL_TOO_LONG,     /* label is too long*/
    ERR_INVALID_OPERAND,    /* invalid operand*/
    ERR_DUPLICATE_LABEL,    /* duplicate label*/
    ERR_OUT_OF_MEMORY,      /* error is out of memory*/
    ERR_SYNTAX,              /* syntax error*/
    ERR_EMPTY_LINE           /*empty line*/
} FrontError;


typedef struct {
    FrontError code;                    /*error code using enum     */
    int         line;                   /*num of line*/
    char        message[MAX_ERROR_MSG]; /*error description*/
} FrontErrorInfo;

void front_error_report(const FrontErrorInfo *err);
char* strdup(const char* s);
#endif /* FRONTEND_ERROR_H */
