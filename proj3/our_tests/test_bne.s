main:

	addi $s0 $0 1 #s0 = 1
	addi $s1 $0 4 #s1 = 4
	bne $s0 $s1 Loop2

Loop:
	addi $s0 $0 6
	beq $s0 $s1 Exit
	addi $s1 $s1 1
	j Loop

Loop2:
	beq $s1 $s0 Loop
	addi $s0 $s0 1
	j Loop2

Exit:
	addi $s0 $0 0
	addi $s1 $0 0
	addi $s2 $0 0

	