#this test currently infinite loop in loopy so idk how to test overflow

main:

	addi $s0, $0, 4 #s0 = 4
	addi $s1, $0 1 #s1 = 1
	j Finish
	
	addi $s1 $0 16 

Finish:
	
	addi  $v0, $0, 13 #v0 = 13	
	add  $s0 $0 $v0 	 #s0 = 13
	addi  $s2 $s2 1		 #s2 = 1
	addi  $s0 $0 32		 #s0 = 32
	addi  $s1 $0 0		 #s1 = 0
	bne  $s1 $s0 Loopy 

	addi $s1 $0 16

Loopy: # this loop SHOULD cause an overflow
	beq $s0 $s1 Exit
	sll $s2 $s2 2
	j Loopy

Exit:
	add $s1 $0 $a1 #s1 = 0
	add $s2 $0 $a2 #s2 = 0
	addi $s0 $0 0 #s0 = 0
	
	