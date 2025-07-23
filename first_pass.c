#include "first_pass.h"
#include "symbol_table.h"
#include <string.h>

/*IC = כמה מילות זיכרון תופסות כל ההוראות
DC = כמה מילות זיכרון תופסים כל הנתונים*/

/* זה המעבר הראשון - פשוט עובר צומת ושולח ל"פרוסס נוד" */
int first_pass(ASTNode* ast_head, SymbolTable* symbols, MemoryManager* memory) {
    memory->IC = 0;
    memory->DC = 0;

    ASTNode* current = ast_head;
    while(current != NULL) {
        process_node(current, symbols, memory);
        current = current->next;
    }

     update_data_symbols(symbols, memory->IC);

    return 0;
}

/*הפונקציה הזו מעבדת את הצומת*/
int process_node(ASTNode* node, SymbolTable* symbols, MemoryManager* memory) {
    /* יש תווית? */
    if(node->label != NULL) { /*טיפול בהנחיות*/
        int is_data = is_directive(node);              /* האם הנחיה */
        int is_extern = is_extern_directive(node);     /* האם חיצונית */
        
        /* כתובת נכונה לפי סוג */
        int adress;
        if(is_data) {
            adress = memory->DC + /* כתובת נתונים */;
        else 
            adress = memory->IC + MEMORY_START;
        
        add_symbol(node->label, adress, is_data, is_extern);
    }


    /* עדכון מונה מתאים */
    if (is_directive(node)) {
        memory->DC += /* גודל נתונים */;
    } else {
        memory->IC += calculate_instruction_size(node);
    }

    return 0;
}

/*בודקת האם תווית מסוימת היא הנחיה*/
int is_directive(ASTNode* node) {
   switch (node->opcode)
   {
   case DIR:
    /* code */
    break;
   
   default:
    break;
   }


