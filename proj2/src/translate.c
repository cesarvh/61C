#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"


/* SOLUTION CODE BELOW */
const int TWO_POW_SEVENTEEN = 131072;    // 2^17

/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li and blt pseudoinstructions. Your pseudoinstruction 
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are 
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number 
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addiu instruction. Otherwise, expand it into 
        a lui-ori pair.

   MARS has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if MARS behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that  
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
  // char* name is the instruction
  // char** args is the array of arguments
  // int num_args is the number of arguments
  // make sure each line in the .out file only contains one instruction (be careful when expanding instructions)
    if (strcmp(name, "li") == 0) { //li = load immediate (li dst, imm). 
        /* YOUR CODE HERE */
      // loads 32 bit immediate into dst
      // split li up into lui ori pair

      if (num_args != 2) {
        return 0;
      }
      char* immediate = args[1];
      char* reg = args[0];
      // convert to integer
      // long int retval = strtol(immediate, NULL, 10);

      // maybe translate to binary or hex
      long int lower_bound = -2147483648; // smallest signed integer
      long int upper_bound = 4294967295; // biggest unsigned integer
      long int retval;
      int x;
      x = translate_num(&retval, immediate, lower_bound, upper_bound); // is this ok (what does it mean by unsigned or signed)
      if (x == -1) {
        return 0;
      }

      if (retval >= lower_bound && retval <= upper_bound) {
        // if immediate can fit in the imm field in addiu? (16 bits),  expand li to addiu
        // long int lower_bound_addiu = −32768;
        long int lower_bound_addiu = -32768;
        long int upper_bound_addiu = 65535;
        char* dst_register = args[0];
        if (retval >= lower_bound_addiu && retval <= upper_bound_addiu) {
          // expand with addiu
          char* zero_reg = "$0";
          fprintf(output, "addiu %s %s %s\n", dst_register, zero_reg, immediate); // new line?
          return 1;
        } else {

          long int lui_val;
          lui_val = retval >> 16;
          long int ori_val;
          uint16_t x = 65535; //2^8 - 1 for 0000000011111111
          ori_val = retval & x;
          char* assembly_reg = "$at";
          fprintf(output, "lui %s %ld\n", assembly_reg, lui_val);
          fprintf(output, "ori %s %s %ld\n", reg, assembly_reg, ori_val);
          return 2;
        }
      }
    } else if (strcmp(name, "move") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 2) {
          return 0;
        }
        char* dst = args[0];
        char* src = args[1];
        char* zero_reg = "$0";
        fprintf(output, "addu %s %s %s\n", dst, src, zero_reg);
        return 1;  
    } else if (strcmp(name, "blt") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }
        char* rs = args[0];
        char* rt = args[1];
        char* label = args[2];
        char* assembly_reg = "$at";
        char* zero_reg = "$0";
        fprintf(output, "slt %s %s %s\n", assembly_reg, rs, rt);
        fprintf(output, "bne %s %s %s\n", assembly_reg, zero_reg, label);
        return 2;

    } else if (strcmp(name, "bgt") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }
        // branch greather than
        char* rs = args[0];
        char* rt = args[1];
        char* label = args[2];
        char* assembly_reg = "$at";
        char* zero_reg = "$0";
        fprintf(output, "slt %s %s %s\n", assembly_reg, rt, rs);
        fprintf(output, "bne %s %s %s\n", assembly_reg, zero_reg, label);
        return 2;

    } else if (strcmp(name, "traddu") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }
        char* rd = args[0];
        char* rs = args[1];
        char* rt = args[2];
        char* assembly_reg = "$at";
        fprintf(output, "addu %s %s %s\n", assembly_reg, rs, rt);
        fprintf(output, "addu %s %s %s\n", rd, rd, assembly_reg);
        return 2;       
    } else if (strcmp(name, "swpr") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 2) {
          return 0;
        }
        char* rd = args[0];
        char* rs = args[1]; // store rd to temp
        // store rs in rd
        // store temp to rs
        char* assembly_reg = "$at";
        char* zero_reg = "$0";
        fprintf(output, "addu %s %s %s\n", assembly_reg, rd, zero_reg);
        fprintf(output, "addu %s %s %s\n", rd, rs, zero_reg);
        fprintf(output, "addu %s %s %s\n", rs, assembly_reg, zero_reg);
        return 3;
    } else if (strcmp(name, "mul") == 0) {
        if (num_args != 3) {
          return 0;
        }
        char* rd = args[0];
        char* rs = args[1];
        char* rt = args[2];
        /* YOUR CODE HERE */
        fprintf(output, "mult %s %s\n", rs, rt);
        // store lower 32 bits into rd
        fprintf(output, "mflo %s\n", rd);
        return 2;       
    } else if (strcmp(name, "div") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }
        char* rd = args[0];
        char* rs = args[1];
        char* rt = args[2];
        
        fprintf(output, "div %s %s\n", rs, rt);
        fprintf(output, "mflo %s", rd);
        return 2;       
    } else if (strcmp(name, "rem") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 3) {
          return 0;
        }
        char* rd = args[0];
        char* rs = args[1];
        char* rt = args[2];

        fprintf(output, "div %s %s\n", rs, rt);
        fprintf(output, "mfhi %s\n", rd);
        return 2;       
    } 
    write_inst_string(output, name, args, num_args);
    return 1;

}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.
   
   NAME is the name of the instruction, 
   ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS. 

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step. If a symbol should be relocated, it should be added to the
   relocation table (RELTBL), and the fields for that symbol should be set to
   all zeros. 

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write 
   anything to OUTPUT but simply return -1. MARS may be a useful resource for
   this step.

   Some function declarations for the write_*() functions are provided in translate.h, and you MUST implement these
   and use these as function headers. You may use helper functions, but you still must implement
   the provided write_* functions declared in translate.h.

   Returns 0 on success and -1 on error. 
 */


int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr,
    SymbolTable* symtbl, SymbolTable* reltbl) {
    if (strcmp(name, "addu") == 0)       return write_rtype (0x21, output, args, num_args);
    else if (strcmp(name, "or") == 0)    return write_rtype (0x25, output, args, num_args);
    else if (strcmp(name, "slt") == 0)   return write_rtype (0x2a, output, args, num_args);
    else if (strcmp(name, "sltu") == 0)  return write_rtype (0x2b, output, args, num_args);
    else if (strcmp(name, "sll") == 0)   return write_shift (0x0, output, args, num_args);
    else if (strcmp(name, "jr") == 0)    return  write_jr(0x08, output, args, num_args);
    else if (strcmp(name, "addiu") == 0) return  write_addiu(0x9, output, args, num_args);

    else if (strcmp(name, "ori") == 0)   return  write_ori(0xd, output, args, num_args); 
    else if (strcmp(name, "lui") == 0)   return  write_lui(0xf, output, args, num_args); 
    else if (strcmp(name, "lb") == 0)    return  write_mem(0x20, output, args, num_args);  

    else if (strcmp(name, "lbu") == 0)   return  write_mem(0x24, output, args, num_args); 
    else if (strcmp(name, "lw") == 0)    return  write_mem(0x23, output, args, num_args); 
    else if (strcmp(name, "sb") == 0)    return  write_mem(0x28, output, args, num_args);  
    else if (strcmp(name, "sw") == 0)    return  write_mem(0x2b, output, args, num_args); 
   
    else if (strcmp(name, "mult") == 0)    return  write_rtype(0x18, output, args, num_args); 
    else if (strcmp(name, "div") == 0)    return  write_rtype(0x1a, output, args, num_args); 
    else if (strcmp(name, "mfhi") == 0)    return  write_rtype(0x10, output, args, num_args); 
    else if (strcmp(name, "mflo") == 0)    return  write_rtype(0x12, output, args, num_args); 

    else if (strcmp(name, "beq") == 0)   return  write_branch(0x4, output, args, num_args,addr, symtbl);
    else if (strcmp(name, "bne") == 0)   return  write_branch(0x5, output, args, num_args,addr, symtbl);
    else if (strcmp(name, "j") == 0)     return  write_jump(0x2, output, args, num_args, addr, reltbl);
    else if (strcmp(name, "jal") == 0)   return  write_jump(0x3, output, args, num_args, addr, reltbl);

    else          return -1;
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to 
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(uint8_t funct, FILE* output, char** args, size_t num_args) {
    /* Too many or not enough arguments*/
    // div or mult
    int rd, rs, rt;
    uint32_t instruction;
    if (funct == 0x18 || funct == 0x1a) {  //div or mult
        if (num_args != 2) {
            return - 1;
        }
        rs = translate_reg(args[0]);
        rt = translate_reg(args[1]);
        instruction = (0 << 26) | (rs << 21) | (rt << 16) | (0<<11) | (0 << 6) | funct;
        write_inst_hex(output, instruction);
        return 0;
      
    } else if (funct == 0x10 || funct == 0x12) { // mfhi or mflo

        if (num_args != 1) {
            return -1;
        }
        rd = translate_reg(args[0]);
        instruction = (0 << 26) | (0 << 21) | (0 << 16) | (rd<<11) | (0 << 6) | funct;
        write_inst_hex(output, instruction);
        return 0;

    } else { // all of the others

        if (num_args != 3) {
            return -1;
        }
        rd = translate_reg(args[0]);
        rs = translate_reg(args[1]);
        rt = translate_reg(args[2]);
        if (rd == -1 || rs == -1 || rt == -1) {
            return -1;
        }

        instruction =  (0 << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (0 << 6) | funct;
        write_inst_hex(output, instruction);
        return 0;
    }
    return 0; 
}

/* A helper function for writing shift instructions. You should use 
   translate_num() to parse numerical arguments. translate_num() is defined
   in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */ 
int write_shift(uint8_t funct, FILE* output, char** args, size_t num_args) {
  // Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }
    long int shamt;
    int rd = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int err = translate_num(&shamt, args[2], 0, 31);

    if (rd == -1 || rt == -1 || err == -1) {
      return -1;
    }

    // if (num_args != 3 || rd == -1 || rt == -1 || err == -1) {
    //   return -1;
    // }
    uint32_t instruction = (0 << 26) | (0 << 21) | (rt << 16) | (rd << 11) | (shamt << 6) | funct ;
    write_inst_hex(output, instruction);
    return 0;
}

/* The rest of your write_*() functions below */
int write_jr(uint8_t funct, FILE* output, char** args, size_t num_args) {
    // check if anyone is -1???
    if (num_args != 1) {
      return -1;
    }
    int rs = translate_reg(args[0]);
    if (rs == -1) {
      return -1;
    }
    uint32_t instruction = (0 << 26) | (rs << 21) | ( 0 << 16) | (0<<6) | funct;
    write_inst_hex(output, instruction);
    return 0;
}

int write_addiu(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
      if (num_args != 3) {
        return -1;
      }

    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], INT16_MIN, INT16_MAX);

    if (rt == -1 || rs == -1 || err == -1) {
      return -1;
    }
    // if (imm < 0) {
      imm = imm & 0xFFFF; // handle overflow or whatever
    // }


    uint32_t instruction = (opcode << 26) | ( rs << 21) | (rt << 16) | imm;
    write_inst_hex(output, instruction);
    return 0;
}

int write_ori(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
  if (num_args != 3) {
    return -1;
  }
    
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], 0, UINT16_MAX);

    if (rt == 1 || rs == -1 || err == -1) {
      return -1;
    }

 
  // this might need fixing
    // if (imm < 0) {
    //   imm = imm * -1;
    // }

    uint32_t instruction = (opcode << 26) | (rs << 21) | (rt << 16) | imm;
    write_inst_hex(output, instruction);
    return 0;
}

int write_lui(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 2) {
      return -1;
    }
    long int imm;
    int rt = translate_reg(args[0]);
    int err = translate_num(&imm, args[1], 0, UINT16_MAX);


    if (rt == -1 || err == -1 || imm < 0) {
      return -1;
    }

    // if (imm < 0) {
      imm = imm & 0xFFFF;
    // }

    uint32_t instruction = (opcode << 26) | (0 << 21) | (rt << 16) |imm;
    write_inst_hex(output, instruction);
    return 0;
}

int write_mem(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }
    long int imm;

    // if (translate_reg(args[0]) == -1) {
    //   return -1;
    // }
    // if (translate_reg(args[2]) == -1) {
    //   return -1;
    // }
    // if (translate_num(&imm, args[1], INT16_MIN, INT16_MAX) == -1) {
    //   return -1;
    // }
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[2]);
    int err = translate_num(&imm, args[1], INT16_MIN, INT16_MAX);
    if (rt == -1 || rs == -1 || err == -1) {
      return -1;
    }

        // if (imm < 0) {
      imm = imm & 0xFFFF; // handle overflow or whatever
    // }


    uint32_t instruction = (opcode << 26) | (rs << 21) | (rt << 16) | imm;
    write_inst_hex(output, instruction);
    return 0;
}

/*  A helper function to determine if a destination address
    can be branched to
*/
static int can_branch_to(uint32_t src_addr, uint32_t dest_addr) {
    int32_t diff = dest_addr - src_addr;
    return (diff >= 0 && diff <= TWO_POW_SEVENTEEN) || (diff < 0 && diff >= -(TWO_POW_SEVENTEEN - 4));
}


int write_branch(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl) {
    // Perhaps perform some error checking?
    if (num_args != 3) {
      return -1;
    }
    // check to see if can branch to address
    
    if (translate_reg(args[0]) == -1 || translate_reg(args[1]) == -1) {
      return -1;
    }
    
    int rs = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int label_addr = get_addr_for_symbol(symtbl, args[2]); // get the address for the label
    // immediate is the offset
    if (label_addr == -1) {
      return -1;
    }
    if (can_branch_to(addr, label_addr) != 1) {
      return -1;
    }

    int32_t offset = ((label_addr - addr - 4) / 4) & 0xFFFF;


    // branching: jump by where i am + offset-> the immediate is the offset
    uint32_t instruction = (opcode << 26) | (rs << 21) | (rt << 16) | offset;
    write_inst_hex(output, instruction);        
    return 0;
}

int write_jump(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* reltbl) {


    if (num_args != 1) { 
      return -1;
    }

    // add symbol with addr to table
    // relocation table stores symbols that need to be relocated
    char* label = args[0];
    add_to_table(reltbl, label, addr); // add to relocation table
    // int32_t offset;
    uint32_t instruction;
    instruction = (opcode << 26);
    write_inst_hex(output, instruction);



    return 0;

}