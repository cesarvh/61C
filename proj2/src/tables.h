#ifndef TABLES_H
#define TABLES_H

#include <stdint.h>

extern const int SYMTBL_NON_UNIQUE;      // allows duplicate names in table
extern const int SYMTBL_UNIQUE_NAME;     // duplicate names not allowed

/* Defined SymbolTable. PLEASE DON'T CHANGE THIS FILE.
 */


typedef struct {
    char *name; // name
    uint32_t addr; // address
} Symbol;

typedef struct {
    Symbol* tbl;
    uint32_t len; // number of symbols in the table
    uint32_t cap; // gives the max capacity (corresponding to amt of allocated memory) of symbol table
    int mode;
} SymbolTable;

/* Helper functions: */

void allocation_failed();

void addr_alignment_incorrect();

void name_already_exists(const char* name);

void write_symbol(FILE* output, uint32_t addr, const char* name);

/* IMPLEMENT ME - see documentation in tables.c */
SymbolTable* create_table(int mode);

/* IMPLEMENT ME - see documentation in tables.c */
void free_table(SymbolTable* table);

/* IMPLEMENT ME - see documentation in tables.c */
int add_to_table(SymbolTable* table, const char* name, uint32_t addr);

/* IMPLEMENT ME - see documentation in tables.c */
int64_t get_addr_for_symbol(SymbolTable* table, const char* name);

/* IMPLEMENT ME - see documentation in tables.c */
void write_table(SymbolTable* table, FILE* output);

#endif
