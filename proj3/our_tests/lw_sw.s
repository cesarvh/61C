addi  $s0 $0 15
addi $s1 $0 31
addi $sp $sp, -12

sw $s0 4($sp)
sw $s1 8($sp)
sw $ra 0($sp)

addi $t0 $0 5
addi $t1 $0 10

add $s0 $s0 $t0
add $s1 $s1 $t1


lw $ra 0($sp)
lw $s0 4($sp) # should be 15
lw $s1 8($sp) #should be 31

addi $sp $sp, 12


