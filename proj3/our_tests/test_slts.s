addi $s0 $0 4
addi $s1 $0 16
slt $s2 $s0 $s1		#if s0 < s1, then s2 is 1, else s2 = 0
					# s2 is 1

slt $s2 $s1 $s0		#s2 is 0

addi $s0 $0 -1		#s0 is -1
addi $s1 $0 -5		#s1 is -5
slt $s2 $s1 $s0		#s2 is 1
slt $s2 $s0 $s1		#s2 is 0

addi $s0 $0 4		#s0 = 4
addi $s1 $0 16		#s1 = 16
sltu $s2 $s0 $s1	#s2 is 1
sltu $s2 $s1 $s0		#s2 is 0

addi $s0 $0 -1		#s0 is -1
slti $s2 $s0 -3		#s2 is 0
slti $s2 $s0 5		#s2 is 1