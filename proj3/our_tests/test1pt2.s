addi $s0 $s0 6
sll $s0 $s0 1		#s0 = 12
sll $s0 $s0 1		#s1 = 24

addi $s1 $s1 31		#s1 = 31
and $s1 $s1 $s0		#s1 = 12
sra $s1 $s1 2		#s1 = 15
# addi $t4 $s2 10
addi $s0 $0 -6
sra $s0 $s0 1

addi $s0 $0 4
addi $s1 $0 3 

or $s0 $s0 $s1		#s0 = 7

ori $s0 $s0 16		#s0 = 10111 in binary
