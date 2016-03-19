# CS 61C Spring 2015 Project 2-2 
# symbol_list.s

#==============================================================================
#                              Project 2-2 Part 2
#                               SymbolList README
#==============================================================================
# In this file you will be implementing a linked list-based data structure for
# storing symbols. Its purpose is analogous to SymbolTable from Proj2-1, but to
# disinguish it we will call it SymbolList.
#
# Each node in the SymbolList contains a (name, addr) pair. An empty SymbolList
# is simply a pointer to NULL. As (name, addr) pairs are added to a SymbolList,
# new list nodes are dynamically allocated and appended to the front of the list. 
# However, as there is no free() in MARS, you do not need to write a free_list()
# function. You may use the functions in string.s during your implementation.
#
# You do not need to perform error checking on duplicate addr or name entries
# in SymbolList, nor do you need to do alignment checking. Don't worry about
# provided addresses being too big.
#
# If SymbolList were to be written in C, each list node would be declared as 
# follows:
#   typedef struct symbollist { 
#       char* name;
#       int addr;
#       struct symbollist* next; 
#   } SymbolList;
#
# You need to write addr_for_symbol() and add_to_list().
# You will find test cases in linker-tests/test_symbol_list.s
#==============================================================================

.include "string.s"

.text

#------------------------------------------------------------------------------
# function addr_for_symbol()
#------------------------------------------------------------------------------
# Iterates through the SymbolList and searches for an entry with the given name.
# If an entry is found, return that addr. Otherwise return -1.
#
# Arguments:
#  $a0 = pointer to a SymbolList (NULL indicates empty list)
#  $a1 = name to look for
#
# Returns:  address of symbol if found or -1 if not found
#------------------------------------------------------------------------------
addr_for_symbol:
        # YOUR CODE HERE
        # while $a0 is not null, get name of symbol at $a0, see if it's equal to $a1.
        # if it is, return the addr, else return -1
  		beq $a0 $zero exit

last_symbol_check:   
		addiu $sp $sp -12
		sw $a0 0($sp)
		sw $a1 4($sp)
		sw $ra 8($sp)
		lw $a0 0($a0) # get name pointer
		jal streq

		#load everything back then increment stack pointer
		lw $a0 0($sp)
		lw $a1 4($sp)
		lw $ra 8($sp)
		addiu $sp $sp 12

		beq $v0 $zero foundname

		# get the name of the symbol at $a0
		# see if it's equal to $a1

# nameloop:
# 		# return address of symbol if found
# 		beq $a1 $0 exit

# 		lb $t1 0($t0) # t1 is the first character
# 		lb $t2 0($a1) # get pointer to the first character of the name

# 		or $t3 $t1 $t2 # check this!!!!!!!!!!!! 
# 		beq $t3 $zero foundname # they're both zero (null)

# 		beq $t1 $zero nextsymbol # do this if lengths are not the same -> do i use t3 or t1??????????? same for below
# 		beq $t2 $zero nextsymbol
# 		# compare the chars in t1 and t2
# 		bne $t1 $t2 nextsymbol
# 		# increment t1 and t2 by 1 to get the next character
# 		addi $t0 $t0 1 # check this!!!!!!!!!!!!
# 		addi $a1 $a1 1
# 		j nameloop


nextsymbol:
		# get the next symbol ($a0 + 8)
		# load it to $t0
		# jump to loop
		# $a0 = struct SymbolList*
		addu $a0 $a0 8
		# $a0 = pointer to next
		lw $a0 0($a0)
		# Dereference pointer to next = next (which is a struct SymbolList*)
		j addr_for_symbol

foundname:
		# return the int address at(?) ($a0 + 4)
		lw $v0 4($a0) # check this!!!!!!!!!
		jr $ra

exit: 	
		addi $v0 $zero -1
		jr $ra
        
#------------------------------------------------------------------------------
# function add_to_list()
#------------------------------------------------------------------------------
# Adds a (name, addr) pair to the FRONT of the list. You should call new_node()
# to create a new node. You do not need to perform error checking on duplicate 
# addr or name entries in SymbolList, nor do you need to do alignment checking.
#
# As with Project 2-1, you WILL need to create a copy of the string that was
# passed in. copy_of_str() from Part 1 will be useful. After new entry has been
# added to the list, return the new list.
#
# Arguments:
#   $a0 = ptr to list (may be NULL)
#   $a1 = pointer to name of symbol (string)
#   $a2 = address of symbol (integer)
#
# Returns: the new list
#------------------------------------------------------------------------------
add_to_list:    
        # YOUR CODE HERE
        # make a new node
        # put the name and address in there
        addiu $sp $sp -16
        sw $a0 0($sp)
        sw $a1 4($sp)
        sw $a2 8($sp)
        sw $ra 12($sp)
        jal new_node

        lw $a0 0($sp)
        lw $a1 4($sp)
        lw $a2 8($sp)
        lw $ra 12($sp)
        addiu $sp $sp 16

        # v0 has a pointer to the new node

        # use copy_of_str to copy string
        # deincrement stack pointer
        # put registers on the stack
        addiu $sp $sp -20  #want to save a0, a1, a2, v0
        sw $a0 4($sp)
        sw $a1 8($sp)
        sw $a2 12($sp)
        sw $ra 16($sp)
        sw $v0 0($sp) # save the node pointer

        addu $a0 $zero $a1
        # use function
        jal copy_of_str

        # move contents of v0 to t1: t1 holds copy of string
        addu $t1 $v0 $0

        # reload everything onto the stack and increment stack pointer
        lw $a0 4($sp)
        lw $a1 8($sp)
        lw $a2 12($sp)
        lw $ra 16($sp)
        lw $v0 0($sp)
        addiu $sp $sp 20


        # move string pointed by t1 to new node
        addiu $sp $sp -24
        sw $v0 0 ($sp)
        sw $a0 4($sp)
        sw $a1 8($sp)
        sw $a2 12($sp)
        sw $ra 16($sp)
        sw $t1 20($sp)

        addu $a0 $t1 $zero # move the string to a0

        jal strlen

        addu $t2 $v0 $0 # t2 has the length of the string

        lw $v0 0 ($sp)
        lw $a0 4($sp)
        lw $a1 8($sp)
        lw $a2 12($sp)
        lw $ra 16($sp)
        lw $t1 20($sp)
        addiu $sp $sp 24

        # move address of the copy (t1) to 0($v0)
        sw $t1 0($v0)

        # move address to new node
        sw $a2 4($v0)

        beq $a0 $zero null_list# if this is true, then you don't have add a next pointer

        # set pointer to list $a0 to be the next of the new node
        sw $a0 8($v0)
        jr $ra


null_list:
        addu $t3 $zero $zero
        sw $t3 8($v0)
        jr $ra

		# load node into v0
		# jr ra

###############################################################################
#                 DO NOT MODIFY ANYTHING BELOW THIS POINT                       
###############################################################################

#------------------------------------------------------------------------------
# function symbol_for_addr()
#------------------------------------------------------------------------------
# Iterates through the SymbolList and searches for an entry with the given addr.
# If an entry is found, return a pointer to the name. Otherwise return NULL.
#
# Arguments:
#  $a0 = pointer to a SymbolList (NULL indicates empty list)
#  $a1 = addr to look for
#
# Returns: a pointer to the name if found or NULL if not found
#------------------------------------------------------------------------------
symbol_for_addr:
        beq $a0, $0, symbol_not_found   # Begin symbol_for_addr
        lw $t0, 4($a0)
        beq $t0, $a1, symbol_found
        lw $a0, 8($a0)
        j symbol_for_addr
symbol_found:
        lw $v0, 0($a0)
        jr $ra
symbol_not_found:
        li $v0, 0
        jr $ra                  # End addr_for_symbol

#------------------------------------------------------------------------------
# function print_list() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = pointer to a SymbolList (NULL indicates empty list)
#  $a1 = print function
#  $a2 = file pointer
#------------------------------------------------------------------------------
print_list:
        addiu $sp, $sp, -16             # Begin print_list
        sw $s0, 12($sp)
        sw $s1, 8($sp)
        sw $s2, 4($sp)
        sw $ra, 0($sp)
        move $s0, $a0
        move $s1, $a1
        move $s2, $a2
print_list_loop:
        beq $s0, $0, print_list_end
        lw $a0, 4($s0)
        lw $a1, 0($s0)
        move $a2, $s2
        jalr $s1
        lw $s0, 8($s0)
        j print_list_loop
print_list_end:
        lw $s0, 12($sp)
        lw $s1, 8($sp)
        lw $s2, 4($sp)
        lw $ra, 0($sp)
        addiu $sp, $sp, 16
        jr $ra                  # End print_list        

#------------------------------------------------------------------------------
# function print_symbol() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Prints one symbol to standard output.
#
# Arguments:
#  $a0 = addr of symbol
#  $a1 = name of symbol
#  $a2 = file pointer (this argument is actually ignored)
#
# Returns: none
#------------------------------------------------------------------------------
print_symbol:
        li $v0, 36                      # Begin print_symbol()
        syscall
        la $a0, tab
        li $v0, 4
        syscall
        move $a0, $a1
        syscall
        la $a0, newline
        syscall
        jr $ra                  # End print_symbol()

#------------------------------------------------------------------------------
# function write_symbol() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Writes one symbol to the file specified
#
# Arguments:
#  $a0 = addr of symbol
#  $a1 = name of symbol
#  $a2 = file pointer
#
# Returns: none
#------------------------------------------------------------------------------
write_symbol:           
        addiu $sp, $sp, -20             # Begin write_symbol()
        sw $s0, 16($sp)
        sw $s1, 12($sp)
        sw $a1, 8($sp)
        sw $a2, 4($sp)
        sw $ra, 0($sp)
        
        la $s0, temp_buf
        
        move $a1, $s0
        jal dec_to_str  # write int
        move $s1, $v0   
        
        addu $a0, $s0, $s1
        addiu $s1, $s1, 1
        la $a1, tab
        li $a2, 1
        jal strncpy             # write tab
        
        lw $a0, 8($sp)
        jal strlen
        
        addu $a0, $s0, $s1
        addu $s1, $s1, $v0
        lw $a1, 8($sp)
        move $a2, $v0
        jal strncpy             # write string
        
        addu $a0, $s0, $s1
        addiu $s1, $s1, 1
        la $a1, newline
        li $a2, 1
        jal strncpy             # write newline
        
        lw $a0, 4($sp)  # file ptr
        move $a1, $s0   # buffer to write
        move $a2, $s1   # num chars to write
        li $v0, 15
        syscall         # write to file
        
        lw $s0, 16($sp)
        lw $s1, 12($sp)
        lw $ra, 0($sp)
        addiu $sp, $sp, 20
        jr $ra                  # End write_symbol()
        
#------------------------------------------------------------------------------
# function new_node() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Creates a new uninitialized SymbolList node.
# Arguments: none
# Returns: pointer to a SymbolList node
#------------------------------------------------------------------------------
new_node:       
        li $a0, 12                      # Begin new_node()
        li $v0, 9
        syscall
        jr $ra                  # End new_node()
        # allocate 12 bytes
        # v0 contains address of allocated memory
        # v0 is a pointer to the symbol node
        
.data 
temp_buf:       .space 1024
