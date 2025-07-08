#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"             
#include "frontend_error.h" 
#include "symbol_table.h"   

/**
 * @brief מבצע בדיקות סמנטיות על ה־AST:
 *        - בונה טבלת סמלים (labels → addresses)
 *        - בודק תוויות כפולות
 *        - בודק טווחי אופראנדים (לדוגמה מונחים עוקבים בזכרון)
 *        - יכול להרחיב לבדיקות נוספות (e.g. שימוש במטריצות)*/
int semantic_check(ASTNode *head, SymbolTable *st, FrontErrorInfo *errors,int max_errors);

#endif /* SEMANTIC_H */
