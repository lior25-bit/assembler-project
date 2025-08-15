/*
*This is the parser, the main part of the first task of the project.
*The parser is supposed to analyze the text and create AST node of the file, every node is a line and every line is made by tokens
*the parser analyze those tokens and report mistakes and if everything is right, and the file is valid it returns head to the list
*/
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "lexer.h"         
#include "ast.h"            
#include "frontend_error.h"  

/**
 * Builds a full AST for an assembly file.
 * Iterates over all lines and returns the head of the AST linked list.
 * Errors are collected in the error module and do not halt processing.
 *
 * fp     Pointer to an open input file.
 * return Head of the ASTNode linked list (or NULL if the file is empty).
 */
ASTNode* parser_parse_file(FILE* fp);

/**
 * Collects all tokens from a single line using the lexer.
 * line         Source string 
 * tokens_arr   Array of tokens (must be allocated by the caller).
 * max_tokens   Maximum number of tokens in the array.
 * return       The number of tokens found in the line.
 */
int parser_gather_tokens(const char* line, Token* tokens_arr, int max_tokens);

/**
 * Converts a token array (representing a single line) to an ASTNode.
 * Checks syntax, detects errors, and fills all fields of the ASTNode.
 *
 * tokens       The token array for the line.
 * num_tokens   Number of tokens in the array.
 * line_number  The original line number in the file.
 * line         The original line string (for error reporting and context).
 * return       Pointer to a fully-initialized ASTNode
 */
ASTNode* parser_parse_tokens(const Token* tokens, int num_tokens, int line_number, const char* original_line);

/**
 * Convenience wrapper: builds an ASTNode for a single line (calls both gather and parse).
 * line:source string (a single line).
 * line_number: The line number in the input file.
 * return:Pointer to a matching ASTNode.
 */
ASTNode* parser_parse_line(const char* line, int line_number);

/**
 * Frees all ASTNodes in a linked list (including freeing all internal fields).
 */
void parser_free_ast(ASTNode* head);


#endif /* PARSER_H */
