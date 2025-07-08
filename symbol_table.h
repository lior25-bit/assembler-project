#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "frontend_error.h"  
#include <stddef.h>          

#define SYMBOL_TABLE_SIZE 256  


typedef struct Symbol {
    char           *name;    /**< שם התווית (הוקצה דינמית) */
    int             address; /**< הכתובת או האוף־סט של התווית */
    struct Symbol  *next;    /**< קישור לסמל הבא באותה משבצת */
} Symbol;

/**
 * @brief טבלת סמלים כמערך של משבצות (cells), כל משבצת מובילה לרשימה של Symbol.
 */
typedef struct {
    Symbol *cells[SYMBOL_TABLE_SIZE];
} SymbolTable;

/**
 * @brief מאתחל טבלת סמלים: מאפס כל משבצת ל-NULL.
 * @param st מצביע לטבלת סמלים לא מאותחלת
 */
void symtab_init(SymbolTable *st);

/**
 * @brief מוסיף סמל חדש (תווית) לטבלת הסמלים.
 
 */
int symtab_add(SymbolTable *st,const char *name,int address,FrontErrorInfo *errors,int max_errors);

/**
 * @brief מחפש סמל לפי שם בתאי הטבלה.
 */
Symbol* symtab_get(const SymbolTable *st,const char *name);

/**
 * @brief משחרר את כל הזיכרון של הסמלים בטבלה.
 */
void symtab_free(SymbolTable *st);

#endif /* SYMBOL_TABLE_H */
