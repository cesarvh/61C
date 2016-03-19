addi $s0 $s0 2147483647
addi $s0 $s0 432343
#shpuld cause overflow

addi $s1 $s1 2147483648
sub $s1 $s1  -44334
#should also cause overflow