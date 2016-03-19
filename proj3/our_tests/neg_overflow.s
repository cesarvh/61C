addi $s0 $s0 -2147483648
addi $s0 $s0 -5534
#should cause an overflow

addi $s1 $s1 -2147483648
sub $s1 $s1  44334
#should also cause an overflow