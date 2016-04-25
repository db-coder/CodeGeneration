	.rdata
S1:
	.ascii "\n"
S2:
	.ascii "\n"
S3:
	.ascii "\n"
	.text
f:
	move $fp $sp
	li $t0 12
	li $v0 1
	move $a0 $t0
	syscall
	li $t0 1
	move $v0 $t0
	move $sp $fp
	jr $ra
main:
	move $fp $sp
	li $t0 1
	sw $t0 0($fp)
	li $t0 2
	sw $t0 4($fp)
	li $t0 3
	sw $t0 8($fp)
	li $t0 -6
	sw $t0 16($fp)
	lw $t0 0($fp)
	mtc1 $t0 $f0
	cvt.s.w $f0 $f0
	addi $sp $sp -4
	mfc1 $t0 $f0
	sw $t0 0($sp)
	lw $t0 4($fp)
	mtc1 $t0 $f0
	cvt.s.w $f0 $f0
	lw $t1 0($sp)
	mtc1 $t1 $f1
	addi $sp $sp 4
	mul.s $f0 $f1 $f0
	addi $sp $sp -4
	mfc1 $t0 $f0
	sw $t0 0($sp)
	lw $t0 8($fp)
	mtc1 $t0 $f0
	cvt.s.w $f0 $f0
	lw $t1 0($sp)
	mtc1 $t1 $f1
	addi $sp $sp 4
	mul.s $f0 $f1 $f0
	addi $sp $sp -4
	mfc1 $t0 $f0
	sw $t0 0($sp)
	lw $t0 16($fp)
	mtc1 $t0 $f0
	cvt.s.w $f0 $f0
	lw $t1 0($sp)
	mtc1 $t1 $f1
	addi $sp $sp 4
	add.s $f0 $f1 $f0
	mfc1 $t0 $f0
	sw $t0 12($fp)
	lw $t0 0($fp)
	mtc1 $t0 $f0
	cvt.s.w $f0 $f0
	li $v0 2
	mov.s $f12 $f0
	syscall
	la $a0 S1
	li $v0 4
	syscall
	lw $t0 12($fp)
	mtc1 $t0 $f0
	cvt.s.w $f0 $f0
	li $v0 2
	mov.s $f12 $f0
	syscall
	la $a0 S2
	li $v0 4
	syscall
	lw $t0 4($fp)
	mtc1 $t0 $f0
	cvt.s.w $f0 $f0
	li $v0 2
	mov.s $f12 $f0
	syscall
	la $a0 S3
	li $v0 4
	syscall
	lw $t0 8($fp)
	mtc1 $t0 $f0
	cvt.s.w $f0 $f0
	li $v0 2
	mov.s $f12 $f0
	syscall
	li $t0 1
	move $v0 $t0
	move $sp $fp
	jr $ra
