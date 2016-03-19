# CS 61C Summer 2015 Project 2-2 
# string.s

#==============================================================================
#                              Project 2-2 Part 1
#                               String README
#==============================================================================
# In this file you will be implementing some utilities for manipulating strings.
# The functions you need to implement are:
#  - strlen()
#  - strncpy()
#  - copy_of_str()
# Test cases are in linker-tests/test_string.s
#==============================================================================

.data
newline:	.asciiz "\n"
tab:	.asciiz "\t"

.text
#------------------------------------------------------------------------------
# function strlen()
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = string input
#
# Returns: the length of the string
#------------------------------------------------------------------------------
strlen:
	add $t2 $a0 $0
	# add $v0 $0 $0 # get our value return readyyyyy
	li $t0 0 # this is our counter
	j Loop
Loop:
	beq $t2 $0 Exit
	lb $t1 0($t2) # load the first item
	beq $t1 $0 Exit
	addi $t0 $t0 1 # increment our counter 
	addi $t2 $t2 1 #increment our pointer
	j Loop

Exit: 
	add $v0 $t0 $0
	jr $ra

#------------------------------------------------------------------------------
# function strncpy()
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = pointer to destination array
#  $a1 = source string
#  $a2 = number of characters to copy
#
# Returns: the destination array
#------------------------------------------------------------------------------
strncpy:
	slti $t5 $a2 0
	bne $t5 $0 done

	li $v0 0
	add $t1 $zero $zero  #counter 
	subu $a2 $a2 1
	beq $a2 $0 done # if a0 then we dont copy anything... done!
	add $t2 $a1 $0 # t3 ---> source
	add $t4 $a0 $0 # pointer that way we can restore at the end
	j cpyLoop #jump to the loop

cpyLoop:
		
	beq $t1 $a2 done # if the counter  == a0, we're done
	lbu $t3 0($t2) 
	beq $t3 $0 done #if we reach the end of the string, we're done
	sb $t3 0($a0) #put t2 into a0
	addi $t2 $t2 1  #advance pointer
	addi $a0 $a0 1 #increment pinter
	addi $t1 $t1 1 #increment count
	beq $t1 $a2 done # if the counter  == a0, we're done
	j cpyLoop  #loop

done: 	    
    sb   $zero 0($a0)  #null terminate
    move $v0 $t4  #move to value register
	jr $ra



#------------------------------------------------------------------------------
# function copy_of_str()
#------------------------------------------------------------------------------
# Creates a copy of a string. You will need to use sbrk (syscall 9) to allocate
# space for the string. strlen() and strncpy() will be helpful for this function.
# In MARS, to malloc memory use the sbrk syscall (syscall 9). See help for details.
#
# Arguments:
#   $a0 = string to copy
#
# Returns: pointer to the copy of the string
#------------------------------------------------------------------------------
copy_of_str:
	addiu $sp $sp -12 # stack pointer
	sw $ra 0($sp) # da ra
	sw $a0 4($sp) # the string which we will be copying out of 
	sw $s0 8($sp) # the string that we will be copying into

	add $s0 $a0 $zero # dest = copy

	# get the length to see how much we will need to make
	jal strlen
	add $a0 $v0 $zero 
	add $t0 $v0 $zero #temp = len
	addi $t0 $t0 1

	addiu $v0, $0, 9
	syscall #allocate the proper ammount of memory

	#prepare the vars to ble able to get strncpy copy
	add $a0 $v0 $zero
	add $a1 $s0 $zero
	add $a2 $t0 $zero
	jal strncpy


	#do the thing with the stack
	lw $ra 0($sp)
	lw $a0 4($sp)
	lw $s0 8($sp)
	addiu $sp $sp 12



	jr $ra

###############################################################################
#                 DO NOT MODIFY ANYTHING BELOW THIS POINT                       
###############################################################################

#------------------------------------------------------------------------------
# function streq() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = string 1
#  $a1 = string 2
#
# Returns: 0 if string 1 and string 2 are equal, -1 if they are not equal
#------------------------------------------------------------------------------
streq:
	beq $a0, $0, streq_false	# Begin streq()
	beq $a1, $0, streq_false
streq_loop:
	lb $t0, 0($a0)
	lb $t1, 0($a1)
	addiu $a0, $a0, 1
	addiu $a1, $a1, 1
	bne $t0, $t1, streq_false
	beq $t0, $0, streq_true
	j streq_loop
streq_true:
	li $v0, 0
	jr $ra
streq_false:
	li $v0, -1
	jr $ra			# End streq()

#------------------------------------------------------------------------------
# function dec_to_str() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Convert a number to its unsigned decimal integer string representation, eg.
# 35 => "35", 1024 => "1024". 
#
# Arguments:
#  $a0 = int to write
#  $a1 = character buffer to write into
#
# Returns: the number of digits written
#------------------------------------------------------------------------------
dec_to_str:
	li $t0, 10			# Begin dec_to_str()
	li $v0, 0
dec_to_str_largest_divisor:
	div $a0, $t0
	mflo $t1		# Quotient
	beq $t1, $0, dec_to_str_next
	mul $t0, $t0, 10
	j dec_to_str_largest_divisor
dec_to_str_next:
	mfhi $t2		# Remainder
dec_to_str_write:
	div $t0, $t0, 10	# Largest divisible amount
	div $t2, $t0
	mflo $t3		# extract digit to write
	addiu $t3, $t3, 48	# convert num -> ASCII
	sb $t3, 0($a1)
	addiu $a1, $a1, 1
	addiu $v0, $v0, 1
	mfhi $t2		# setup for next round
	bne $t2, $0, dec_to_str_write
	jr $ra			# End dec_to_str()
