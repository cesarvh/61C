	.text
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
	.type	main, @function
main:
	lui	$6,%hi(source)
	lw	$3,%lo(source)($6) # loads the first elem of the source array
	beq	$3,$0,$L2 # if 3 is 0, then you go to L2, otherwise you continue
	lui	$7,%hi(dest)
	addiu	$7,$7,%lo(dest)
	addiu	$6,$6,%lo(source)
	move	$2,$0 # this is k = 0
$L3:
	addu	$5,$7,$2 #moves pointer to kth elem of destination
	addu	$4,$6,$2 #moves pointer to kth element of source
	sw	$3,0($5) # saves kth element into the destination
	lw	$3,4($4) # loads the next element 
	addiu	$2,$2,4 # incrementing of the k
	bne	$3,$0,$L3  # if its not 0, then do it all over again
$L2:
	move	$2,$0
	j	$31
	.end	main
	.size	main, .-main
	.globl	source
	.data
	.align	2
	.type	source, @object
	.size	source, 28
source: # this is the array of integers that we will use i thik
	.word	3
	.word	1
	.word	4
	.word	1
	.word	5
	.word	9
	.word	0

	.comm	dest,40,4
	.ident	"GCC: (GNU) 4.4.1"
