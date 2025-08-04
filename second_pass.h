#ifndef SECOND_PASS_H
#define SECOND_PASS_H


int second_pass(ASTNode* ast_head, MemoryManager* memory);
int process_node_secend_pass(AstNode* node, MemoryManager* memory);
    void add_arg_code(int adress);
/*
int process_node_second_pass(ASTNode* node, SymbolTable* symbols, MemoryManager* memory);
/*
