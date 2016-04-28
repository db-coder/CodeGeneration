	.rdata
F1:
	.float 1.7
S1:
	.ascii "\n"
S2:
	.ascii "\n"
S3:
	.ascii "\n"
S4:
	.ascii "\n"
S5:
	.ascii "\n"
S6:
	.ascii "\n"
	.text
main:
	move $fp $sp
	l.s $f1 F1
	mfc1 $t1 $f1
	sw $t1 8($fp)
	li $t0 2
	sw $t0 12($fp)
	lw $t0 8($fp)
	sw $t0 0($fp)
	lw $t0 12($fp)
	sw $t0 4($fp)
	li $t0 2
	sw $t0 20($fp)
	li $t0 7
	sw $t0 36($fp)
	la $a0 S1
	li $v0 4
	syscall
	lw $t0 20($fp)
	li $v0 1
	move $a0 $t0
	syscall
	la $a0 S2
	li $v0 4
	syscall
	lw $t0 36($fp)
	li $v0 1
	move $a0 $t0
	syscall
	la $a0 S3
	li $v0 4
	syscall
	lw $t0 8($fp)
	mtc1 $t0 $f0
	li $v0 2
	mov.s $f12 $f0
	syscall
	la $a0 S4
	li $v0 4
	syscall
	lw $t0 12($fp)
	li $v0 1
	move $a0 $t0
	syscall
	la $a0 S5
	li $v0 4
	syscall
	lw $t0 0($fp)
	mtc1 $t0 $f0
	li $v0 2
	mov.s $f12 $f0
	syscall
	la $a0 S6
	li $v0 4
	syscall
	lw $t0 4($fp)
	li $v0 1
	move $a0 $t0
	syscall
	li $t0 1
	move $v0 $t0
	move $sp $fp
	jr $ra
