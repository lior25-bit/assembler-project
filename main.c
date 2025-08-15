#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  "macro.h"
#include "parser.h"
#include "ast.h"

void print_ast(ASTNode *head) {
    ASTNode *curr = head;
    while (curr != NULL) {
        print_ast_node(curr); 
        curr = curr->next;
    }
}

int main(int argc, char **argv) {
    Macro macro_table[MAX_MACROS];
    int macro_count = 0;
    char am_path[FILENAME_MAX];
    FILE *fp;
    ASTNode *head;

    if (argc != 2) {
        printf( "usage: %s <basename>\n", argv[0]);
        printf("example: %s input1   (expects input1.as)\n", argv[0]);
        return 1;
    }
    run_macro_processor(argv[1], macro_table, &macro_count);

    
    strcpy(am_path, argv[1]);
    strcat(am_path, ".am");

    fp = fopen(am_path, "r");
    if (!fp) { perror(am_path); return 1; }

    head = parser_parse_file(fp);
    fclose(fp);

    if (!head) { puts("No AST generated."); return 0; }

    print_ast(head);      
    parser_free_ast(head);   
    return 0;
}

