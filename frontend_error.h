#ifndef FRONTEND_ERROR_H
#define FRONTEND_ERROR_H

#define MAX_ERROR_MSG 128  /* אורך מקסימלי של הודעת שגיאה */

/**
 * @brief קודי השגיאות האפשריים בשלב ה-Front-End.
 */
typedef enum {
    ERR_UNEXPECTED_TOKEN,   /* טוקן לא צפוי */
    ERR_LABEL_TOO_LONG,     /* תווית ארוכה מדי */
    ERR_INVALID_OPERAND,    /* אופראנד לא תקין */
    ERR_TOO_MANY_OPERANDS,  /* יותר מדי אופראנדים בשורה */
    ERR_DUPLICATE_LABEL,    /* תווית כבר קיימת */
    ERR_OUT_OF_MEMORY,      /* שגיאת הקצאת זיכרון */
    ERR_SYNTAX              /* שגיאת תחביר כללית */
} FrontError;

/**
 * @brief כל פרטי השגיאה שקרתה.
 */
typedef struct {
    FrontError code;                    /**< קוד השגיאה       */
    int         line;                   /**< מספר השורה בקובץ */
    char        message[MAX_ERROR_MSG]; /**< תיאור השגיאה     */
} FrontErrorInfo;

/**
 * @brief מדפיס את פרטי השגיאה ל-stderr.
 * @param err מצביע למבנה עם פרטי השגיאה.
 */
void front_error_report(const FrontErrorInfo *err);

#endif /* FRONTEND_ERROR_H */
