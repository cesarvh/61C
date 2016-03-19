addi $s0 $s0 7
addi $s1 $s1 7
multu $s1 $s0

mfhi $s0 # should be 0 
mflo $s1 # should be 49 (0x00000031)


addi $s0 $s0 65535 # (0xFFFF)
addi $s1 $s1 65486 # should be 0xFFFF
multu $s1 $s0 

mfhi $s0 # should be 0x00000000
mflo $s1 # should be 0xfffe0001 (overflow!)