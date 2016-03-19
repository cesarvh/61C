main:

	addi $t0 $s0 0 #t0 = 0
	addi $t1 $s1 1 #t1 = 1
	
	add $t2 $t1 $t0 #t2 = 1
	add $t3 $t1 $t2 #t3 = 2
	add $t4 $t2 $t3 #t4 = 3
	add $t5 $t3 $t4 #t5 = 5
	add $t6 $t5 $t4 #t6 = 8
	add $t7 $t6 $t5 #t7 = 13
	# j Finish

# Finish:
	addi $s1 $0 0 #s1 = 0
	addi $s2 $0 0 #s2 = 0
	addi $s0 $0 0 #$s0 = 0
	
	add  $v0 $t7 $zero #v0 = 13	
	add  $s0 $0 $v0 	 #s0 = 13 // STOPS HERE
	addi  $a2 $a2 1		 #a2 = 1
	addi  $a0 $0 32		 #a0 = 32
	addi  $a1 $0 0		 #a1 = 0
	bne  $s1 $s0 Loopy 
	
Loopy: # this loop SHOULD cause an overflow
	beq $a0 $a1 Exit
	sll $a2 $a2 2
	j Loopy 
Exit:
	add $s1 $0 $a1 #s1 = 0
	add $s2 $0 $a2 #s2 = 0
	addi $s0 $0 0 #s0 = 0
	
	