	.data
x:  .word 3

.text
main:
lw $t0, x
add $t0, $s0, $zero
j Exit

Exit:
addi    $a0, $t0, 0
		li      $v0, 1		# you will be asked about what the purpose of this line for syscall 
		syscall			
		li      $v0, 10		
		syscall			
