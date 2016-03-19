addi $s0 $s0 49 #s0 = 49
addi $s1 $s1 7 #s1 = 7
div $s0 $s1 #hi = 0, lo = 7

mfhi $s0 # should be 0
mflo $s1 # should be 7 

sub $s0 $s0 $s0
sub $s1 $s1 $s1

addi $s0 $s0 49 # 
addi $s1 $s1 3 #should be 3 
div $s0 $s1 #high = 1, lo = 16

mfhi $s0 # should be 1
mflo $s1 # should be 16 

