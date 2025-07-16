#ifndef LEXER_H
#define LEXER_H
#define MAX_TOKEN_LEN 64
#include "frontend_error.h"  /* לדיווח שגיאות */
#include <stddef.h>          /* ל-NULL, size_t */

typedef enum {
    TOK_LABEL,      /**< תווית (עם ':' בסיום) */
    TOK_MNEMONIC,   /**< שמות הוראות (mnemonic) */
    TOK_REGISTER,   /**< רגיסטר (למשל R0, R13) */
    TOK_NUMBER,     /**< מספר (decimal או 0x hex) */
    TOK_DIRECTIVE,  /**< דירקטיבה (.data, .string וכו') */
    TOK_STRING,  /**< ערך מחרוזת - "Hello world" */
    TOK_SEPARATOR,  /**< מפרידים (',' ':' ';' וכו') */
    TOK_EOF,         /**< סוף שורה (או קובץ) */
    TOK_MATRIX,
    TOK_INVALID
} TokenType;


typedef struct {
    TokenType type;               /**< סוג הטוקן */
    char      text[MAX_TOKEN_LEN];/**< הטקסט המדויק של הטוקן */
    int       line;               /**< מספר השורה במקור (לדיווח שגיאות) */
} Token;

/**
 * @brief מאתחל את הלֶקְסֶר לשורה חדשה.
 * @param line מחרוזת מקור (יש לשמור בתוקף לאורך כל השימוש).
 */
void   lexer_init_line(const char *line);

/**
 * @brief מחזיר את הטוקן הבא מהשורה.
 * @return מצביע ל־Token חדש (יש לשחרר עם lexer_free_token), או NULL בסוף הקלט.
 */
Token* lexer_next_token(void);

/**
 * @brief משחרר את הזיכרון שהוקצה עבור טוקן.
 * @param tok מצביע ל־Token שהוחזר מ־lexer_next_token
 */
void   lexer_free_token(Token *tok);

int is_label(const char *token_text);
int is_register(const char *token_text);
int is_mnemonic(const char *token_text);
int is_number(const char *token_text);
int is_directive(const char *token_text);
int is_matrix(const char *token_text);
int valid_len(const char *token_text, int num);
#endif /* LEXER_H */