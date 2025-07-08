#ifndef PARSER_H
#define PARSER_H

#include "ast.h"             /* מגדיר את ASTNode, OpCode, Operand וכו' */
#include "frontend_error.h"  /* מגדיר FrontErrorInfo ודיווח שגיאות */
#include "lexer.h"           /* מגדיר Token, lexer_init_line, lexer_next_token */

/*	פותח ופורש קובץ שורה־שורה, משתמש ב־lexer + parse_statement, בונה linked list ומחזיר את ה-head.*/
ASTNode* parser_parse_file(const char *filename);

/*משחרר את כל ה-ASTNode באמצעות free_node (מה־ast.h) ברצף לפי כיוון ה-next*/
void parser_free_ast(ASTNode *head);

/*הליבה: קורא טוקנים ל־label, opcode, operands, בונה ASTNode חדש, מחזיר אותו.*/
ASTNode* parse_statement(void);

/*מטפל במצב שבו ההתחלה היא תווית (LABEL:), מוחק את ה־':', מחזיר מחרוזת נקייה.*/
char* parse_label(Token **ptok);

/*ממיר מחרוזת מ־Token ל־ערך המתאים ב־OpCode enum, מדווח על טקסט לא מוכר.*/
OpCode parse_opcode(Token *tok, FrontErrorInfo *err);

/*בהתאם ל־tok->type בונה Operand מתאים (immediate, register, direct, matrix).*/
Operand parse_operand(Token *tok);

/*אוסף עד שני operands, מדלג על מפרידים, ודואג לשגיאת “יותר מדי אופראנדים”.*/
void parse_operands(ASTNode *node);

#endif /* PARSER_H */
