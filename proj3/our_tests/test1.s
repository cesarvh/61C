addi $s0 $0 27 		#s0 = 27
addi $s1 $0 7  		#s1 = 7
sub $s1 $s0 $s1 	#s1 = 20
subu $s1 $s0 $s1	#s1 = 7

addi $s0 $0 6		#s0 = 6
addi $s1 $0 7		#s1 = 7
multu $s0 $s1		#hi = 0, lo = 42
mflo $s1			#s1 = 42
divu $s1 $s0		#lo = 7 hi = 0
mfhi $s0			#s0 = 0
mflo $s0			#s0 = 7

addi $s1 $0 6
divu $s0 $s1		#hi = 1 lo = 1
mfhi $s0			#s0 = 1
mflo $s1			#s1 = 1