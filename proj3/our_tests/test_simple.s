addi $t1 $0 27 		#t1 = 27
addi $t2 $0 7  		#t2 = 7
sub $t3 $t1 $t2 	#t3 = 20
add $s0 $s0 $t3		#s0 = 20	
subu $t3 $t3 $t1	#t3 = -7

addi $t4 $0 6		#t4 = 6
addi $t5 $0 7		#t5 = 7
addi $s1 $t5 0 		#s1 = 7
multu $t4 $t5		#s1 = 42 (hi = 320b0, lo = 0101010)
divu $s1 $t4		#s1 = 7	(hi = 1, lo = 1)


sll $t4 $t4 1		#t4 = 12
sll $t4 $t4 1		#t4 = 24
add $s2 $0	$t4		#s2 = 24 

subu $s0 $s0 $s0 	#s0 = 0
subu $s1 $s1 $s1 	#s1 = 0
subu $s2 $s2 $s2 	#s2 = 0


and $t4 $0 $t4		#t4 = 0 AND 28 == 0O
add $s0 $0 $t4 		#s0 = 10

ori $t4 $t4 5		#t4 = 5
addi $s2 $s2 30		#s2 = 30
or $s2 $s2 $t4		#s2 = 30 OR 15 == 31 (0x1f)

