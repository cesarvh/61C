main:
	addi $s0 $0 10 #s0 = 10 
	addi $s1 $0 15 #s1 = 15
	addi $s2 $0 30 #s2 = 30

	sll $s0 $s0 2 #s0  = 40
	srl $s1 $s1 2 #s1 = 3
	sra $s2 $s2 2 #s2 = 7

	addi $t1 $0 10 #t1 = 10
	addi $t2 $0 15 #t2 = 15
	addi $t3 $0 30 #t3 = 30


	add $s0 $s0 $t1 #s0 = 50
	add $s1 $s1 $t2 #s1 = 18
	add $s2 $s1 $t3 #s2 = 48

	and $s0 $s0 $s1 #s0 = 18
	or $s1 $s0 $s2 #s1 = 50
	and $s2 $s1 $s2 #s2 = 48

	add $s1 $s2 $s0 #s1 = 66
	addi $s1 $0 0 #s1 = 0
	addi $s2 $0 0 #s1 = 0
	add $s0 $s0 -10
	j Loop

Loop: #loop until $s1 < $s0 (0 < 8)
	slt $t1 $s1 $s0
	beq $t1 $s2 Exit
	addi $s1 $s1 1
	j Loop

Exit:



