main:

	addi $s0 $0 1
	addi $s1 $0 6
	jal blah

	j exit

blah:
	addiu $sp $sp -12
	sw $ra 0($sp)
	sw $s0 4($sp)
	sw $s1 8($sp)

	addi $s0 $0 5
	addi $s1 $0 10

	lw $ra 0($sp)
	lw $s0 4($sp)
	lw $s1 8($sp)

	jr $ra 

exit:
	addi $s0 $0 0
	addi $s1 $0 0
