
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "tables.h"

const int SYMTBL_NON_UNIQUE = 0;
const int SYMTBL_UNIQUE_NAME = 1;

#define INITIAL_SIZE 5
#define SCALING_FACTOR 2

/*******************************
 * Helper Functions
 *******************************/

void allocation_failed() {
    write_to_log("Error: allocation failed\n");
    exit(1);
}

void addr_alignment_incorrect() {
    write_to_log("Error: address is not a multiple of 4.\n");
}

void name_already_exists(const char* name) {
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

void write_symbol(FILE* output, uint32_t addr, const char* name) {
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containing 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time. 
   If memory allocation fails, you should call allocation_failed(). 
   Mode will be either SYMTBL_NON_UNIQUE or SYMTBL_UNIQUE_NAME. You will need
   to store this value for use during add_to_table().
 */
SymbolTable* create_table(int mode) {
    /* YOUR CODE HERE */
  SymbolTable* ptr;
  ptr = (SymbolTable*) malloc(sizeof(SymbolTable));
  if (ptr == NULL) {
    allocation_failed();
  }
  ptr->len = 0;
  ptr->cap = INITIAL_SIZE;
  ptr->mode = mode;
  // SymbolTable table;
  // table.len = 0;
  // table.cap = INITIAL_SIZE;
  // table.mode = mode;
  // ptr = &table;
  // do you allocate memory for the symbol ptr
  Symbol* symbolptr;
  symbolptr = (Symbol*) malloc(INITIAL_SIZE * sizeof(Symbol));
  if (symbolptr == NULL) {
    allocation_failed();
  }
  ptr->tbl = symbolptr;
  return ptr;
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable* table) {
    /* YOUR CODE HERE */
    int table_size = table->len;
    int i = 0;
    while(i < table_size) {
      free((table->tbl)[i].name);
      i++;
    }
  free(table->tbl);
  free(table);

}
/* A suggested helper function for copying the contents of a string. */
static char* create_copy_of_str(const char* str) {
    size_t len = strlen(str) + 1;
    char *buf = (char *) malloc(len);
    if (!buf) {
       allocation_failed();
    }
    strncpy(buf, str, len); 
    return buf;
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   ADDR is given as the byte offset from the first instruction. The SymbolTable
   must be able to resize itself as more elements are added. 

   Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   If ADDR is not word-aligned, you should call addr_alignment_incorrect() and
   return -1. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. If memory
   allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable* table, const char* name, uint32_t addr) {
    /* YOUR CODE HERE */
    char* name_copy = create_copy_of_str(name);
    // if addr is not word aligned call addr_aligmnemt incorrect return -1
    // if table's mode is SYMBL_UNIQUE_NAME and NAME already exists in table, call name_already_exists, return -1
    // if memory allocation fails, call allocation_failed
    // else store symbol name and address, resize symboltable as more elements added
    int word_aligned = addr % 4; // idk about this
    if (word_aligned != 0) {
      addr_alignment_incorrect();
      return -1;
    }
    int table_mode = table->mode;
    Symbol* symbol_array = table->tbl;
    if (table_mode == SYMTBL_UNIQUE_NAME) {
      // check to see if NAME exists in table
      uint32_t indexer = 0;
      uint32_t num_symbols = table->len;
      while (num_symbols > 0) {
        int x = strcmp(symbol_array[indexer].name, name);
        // printf("%d\n", x);
        if (x == 0) {
          name_already_exists(name);
          return -1;
        }
        num_symbols -= 1;
        indexer += 1;
      }
    }
    // store symbol name and address, resize symboltable as more elements are added
    // resize table, add element
    // scaling factor?

    // if symboltable's length = cap, double the size of the table
    uint32_t indexer2 = 0;
    uint32_t symboltable_length = table->len;
    uint32_t symboltable_cap = table->cap;
    if (symboltable_length == symboltable_cap) {
      // resize the table by creating new memory
      // add all the symbols back in
      // free the old memory
      Symbol* new_symbol_ptr = (Symbol*) malloc(symboltable_cap * SCALING_FACTOR * sizeof(Symbol));

      // copy everything in the new table
      while (symboltable_length > 0) {
        new_symbol_ptr[indexer2] = symbol_array[indexer2];
        indexer2 += 1;
        symboltable_length -= 1;
      }
      free(table->tbl);
      table->tbl = new_symbol_ptr;
      //update symbol table cap
      table->cap = symboltable_cap * SCALING_FACTOR;

    }
    // add element in
    Symbol* new_symbol_array = table->tbl;
    new_symbol_array[table->len].name = name_copy;
    new_symbol_array[table->len].addr = addr;

    // update the table's length?
    uint32_t table_length = table->len;
    table->len = table_length + 1;

    return 0;
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable* table, const char* name) {
    /* YOUR CODE HERE */
    // if the symbol with the name is in the table
    // return the address of the given symbol 
    Symbol* symbol_array = table->tbl;
    uint32_t indexer = 0;
    uint32_t num_symbols = table->len;
    while (num_symbols > 0) {
      int x = strcmp(symbol_array[indexer].name, name);
      if (x == 0) { // they're equal
        return symbol_array[indexer].addr; // does this even make sense tho???????????? what exactly is addr
      }
      num_symbols -= 1;
      indexer += 1;
      
    }
    return -1;
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_symbol() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable* table, FILE* output) {
    /* YOUR CODE HERE */
  Symbol* symbol_array = table->tbl;
  uint32_t num_symbols = table->len;
  uint32_t indexer = 0;
  while (num_symbols > 0) {
    Symbol x = symbol_array[indexer];
    int32_t address = x.addr;
    char* symbol_name = x.name;
    write_symbol(output, address, symbol_name);
    indexer += 1;
    num_symbols -= 1;
  }
}
