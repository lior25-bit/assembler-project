/* This file manage file exportation */
#include "file_export.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* internal helpers */
static void write_ob_body(MemoryManager* memory, FILE* fp);
static void write_ob_code(MemoryManager* memory, FILE* fp);
static void write_ob_data(MemoryManager* memory, FILE* fp);
static void safe_fputs_line(FILE* fp, const char* s);
static void free_if_needed(char* p);

/* export .ob */
int export_ob_file(char* file_name, MemoryManager* memory)
{
    char* out;
    FILE* fp;
    int code_count;
    int data_count;

    if (!file_name || !memory) return 0;

    out = create_file_name(file_name, "ob");
    if (!out) return 0;

    fp = fopen(out, "w");
    if (!fp) { free(out); return 0; }

    code_count = memory->IC - MEMORY_START;
    if (code_count < 0) code_count = 0;
    data_count = memory->DC;

    fprintf(fp, "%d %d\n", code_count, data_count);
    write_ob_body(memory, fp);

    fclose(fp);
    free(out);
    return 1;
}

/* export .ent */
int export_ent_file(char* file_name, SymbolTable* table)
{
    char* out;
    FILE* fp;
    Symbol* cur;
    int wrote;

    if (!file_name || !table) return 0;
    if (!has_entry_symbols(table)) return 1;

    out = create_file_name(file_name, "ent");
    if (!out) return 0;

    fp = fopen(out, "w");
    if (!fp) { free(out); return 0; }

    wrote = 0;
    cur = table->head;
    while (cur) {
        if (cur->is_entry) {
            fprintf(fp, "%s\t%d\n", cur->name, cur->address);
            wrote = 1;
        }
        cur = cur->next;
    }

    fclose(fp);
    free(out);
    return wrote ? 1 : 1;
}

/* export .ext */
int export_ext_file(char* file_name, MemoryManager* memory)
{
    char* out;
    FILE* fp;
    ExternUsage* cur;
    int wrote;

    if (!file_name || !memory) return 0;
    if (!has_extern_references(memory)) return 1;

    out = create_file_name(file_name, "ext");
    if (!out) return 0;

    fp = fopen(out, "w");
    if (!fp) { free(out); return 0; }

    wrote = 0;
    cur = memory->extern_list;
    while (cur) {
        fprintf(fp, "%s\t%d\n", cur->symbol_name, cur->address);
        wrote = 1;
        cur = cur->next;
    }

    fclose(fp);
    free(out);
    return wrote ? 1 : 1;
}

/* export .am */
int export_am_file(char* file_name, ASTNode* ast_head)
{
    char* out;
    FILE* fp;
    ASTNode* cur;

    if (!file_name) return 0;

    out = create_file_name(file_name, "am");
    if (!out) return 0;

    fp = fopen(out, "w");
    if (!fp) { free(out); return 0; }

    cur = ast_head;
    while (cur) {
        if (cur->original_line && cur->original_line[0]) {
            safe_fputs_line(fp, cur->original_line);
        }
        cur = cur->next;
    }

    fclose(fp);
    free(out);
    return 1;
}

/* export all */
int export_all_files(char* file_name, MemoryManager* memory, SymbolTable* table, ASTNode* expanded_ast)
{
    int ok;

    if (!file_name || !memory || !table) return 0;

    ok = export_ob_file(file_name, memory);
    if (!ok) return 0;

    ok = export_ent_file(file_name, table);
    if (!ok) return 0;

    ok = export_ext_file(file_name, memory);
    if (!ok) return 0;

    if (expanded_ast) {
        ok = export_am_file(file_name, expanded_ast);
        if (!ok) return 0;
    }
    return 1;
}

/* helpers */
char* create_file_name(char* file_name, char* output_type)
{
    size_t n1, n2;
    char* out;
    if (!file_name || !output_type) return NULL;
    n1 = strlen(file_name);
    n2 = strlen(output_type);
    out = (char*)malloc(n1 + 1 + n2 + 1);
    if (!out) return NULL;
    strcpy(out, file_name);
    out[n1] = '.';
    strcpy(out + n1 + 1, output_type);
    return out;
}

/* convert 10-bit word to fixed 5-digit base-4 string */
char* convert_to_base_four(int value)
{
    char* s;
    int i;
    int v;

    s = (char*)malloc(5 + 1);
    if (!s) return NULL;

    v = value & VALUE_MASK; /* 10 bits */
    for (i = 4; i >= 0; i--) {
        s[i] = (char)('0' + (v & 0x3));
        v >>= 2;
    }
    s[5] = '\0';
    return s;
}

int has_entry_symbols(SymbolTable* table)
{
    Symbol* cur;
    if (!table) return 0;
    cur = table->head;
    while (cur) {
        if (cur->is_entry) return 1;
        cur = cur->next;
    }
    return 0;
}

int has_extern_references(MemoryManager* memory)
{
    if (!memory) return 0;
    return (memory->extern_list != NULL);
}

/* internals */
static void write_ob_body(MemoryManager* memory, FILE* fp)
{
    write_ob_code(memory, fp);
    write_ob_data(memory, fp);
}

static void write_ob_code(MemoryManager* memory, FILE* fp)
{
    int addr;
    int end;
    end = memory->IC;
    if (end < MEMORY_START) end = MEMORY_START;

    for (addr = MEMORY_START; addr < end; addr++) {
        char* b4 = convert_to_base_four(memory->code_image[addr]);
        if (b4) {
            fprintf(fp, "%04d\t%s\n", addr, b4);
            free(b4);
        } else {
            fprintf(fp, "%04d\t00000\n", addr);
        }
    }
}

static void write_ob_data(MemoryManager* memory, FILE* fp)
{
    int i;
    int addr;
    addr = memory->IC;
    for (i = 0; i < memory->DC; i++, addr++) {
        char* b4 = convert_to_base_four(memory->data_image[i]);
        if (b4) {
            fprintf(fp, "%04d\t%s\n", addr, b4);
            free(b4);
        } else {
            fprintf(fp, "%04d\t00000\n", addr);
        }
    }
}

static void safe_fputs_line(FILE* fp, const char* s)
{
    size_t n;
    if (!s) return;
    n = strlen(s);
    if (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        fputs(s, fp);
    } else {
        fputs(s, fp);
        fputc('\n', fp);
    }
}

static void free_if_needed(char* p)
{
    if (p) free(p);
}
