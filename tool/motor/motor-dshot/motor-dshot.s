	.file	"motor-dshot.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"motor-dshot.c"
.LC1:
	.string	"motorMax < DSHOT_NUM_PINS"
.LC2:
	.string	"debug Dshot Frame failure."
	.text
	.p2align 4
	.globl	dshotSendFrames
	.type	dshotSendFrames, @function
dshotSendFrames:
.LFB71:
	.cfi_startproc
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$136, %rsp
	.cfi_def_cfa_offset 192
	movq	%fs:40, %rax
	movq	%rax, 120(%rsp)
	xorl	%eax, %eax
	cmpl	$26, %esi
	jg	.L2
	movq	%rdi, %r11
	movl	%esi, %ebx
	movq	%rdx, %r9
	testl	%esi, %esi
	jle	.L36
	movl	$0, 24(%rsp)
	movslq	%esi, %r8
	movq	%rdi, %rax
	movl	$1, %esi
	leaq	(%rdi,%r8,4), %rdi
	.p2align 4,,10
	.p2align 3
.L5:
	movl	(%rax), %ecx
	movl	%esi, %edx
	addq	$4, %rax
	sall	%cl, %edx
	orl	%edx, 24(%rsp)
	cmpq	%rdi, %rax
	jne	.L5
.L4:
	leaq	48(%rsp), %rax
	salq	$2, %r8
	xorl	%r10d, %r10d
	movq	%rax, 16(%rsp)
.L11:
	testl	%ebx, %ebx
	jle	.L6
	movl	$32768, %edi
	movl	%r10d, %ecx
	movl	$1, %ebx
	movl	$32768, %ebp
	sarl	%cl, %edi
	.p2align 4,,10
	.p2align 3
.L10:
	xorl	%eax, %eax
	xorl	%esi, %esi
	.p2align 4,,10
	.p2align 3
.L8:
	movl	%edi, %edx
	andl	(%r9,%rax), %edx
	jne	.L7
	movl	(%r11,%rax), %ecx
	movl	%ebx, %edx
	sall	%cl, %edx
	orl	%edx, %esi
.L7:
	addq	$4, %rax
	cmpq	%r8, %rax
	jne	.L8
	movq	16(%rsp), %rax
	movl	%esi, (%rax,%r10,4)
	addq	$1, %r10
	cmpq	$16, %r10
	je	.L9
	movl	%ebp, %edi
	movl	%r10d, %ecx
	sarl	%cl, %edi
	jmp	.L10
.L6:
	movq	16(%rsp), %rax
	movl	$0, (%rax,%r10,4)
	addq	$1, %r10
	cmpq	$16, %r10
	jne	.L11
.L9:
	leaq	112(%rsp), %rax
	movl	$10, 28(%rsp)
	leaq	32(%rsp), %rbx
	movq	dshotGpio(%rip), %r15
	movq	%rax, 8(%rsp)
	.p2align 4,,10
	.p2align 3
.L18:
	movq	%rbx, %rsi
	movl	$4, %edi
	call	clock_gettime@PLT
	movq	16(%rsp), %r14
	imulq	$1000000000, 32(%rsp), %rbp
	addq	40(%rsp), %rbp
	leaq	4000(%rbp), %r13
	leaq	2000(%rbp), %r12
	.p2align 4,,10
	.p2align 3
.L12:
	movq	%rbx, %rsi
	movl	$4, %edi
	call	clock_gettime@PLT
	imulq	$1000000000, 32(%rsp), %rax
	addq	40(%rsp), %rax
	cmpq	%rax, %r12
	jg	.L12
	movl	24(%rsp), %ecx
	subq	%r12, %rax
	movl	%ecx, 28(%r15)
	cmpq	$2000, %rax
	jg	.L13
	.p2align 4,,10
	.p2align 3
.L14:
	movq	%rbx, %rsi
	movl	$4, %edi
	call	clock_gettime@PLT
	imulq	$1000000000, 32(%rsp), %rax
	addq	40(%rsp), %rax
	cmpq	%rax, %r13
	jg	.L14
	movl	(%r14), %edx
	subq	%r13, %rax
	movl	%edx, 40(%r15)
	cmpq	$2000, %rax
	jg	.L13
	addq	$6000, %rbp
	.p2align 4,,10
	.p2align 3
.L15:
	movq	%rbx, %rsi
	movl	$4, %edi
	call	clock_gettime@PLT
	imulq	$1000000000, 32(%rsp), %rax
	addq	40(%rsp), %rax
	cmpq	%rax, %rbp
	jg	.L15
	movl	24(%rsp), %ecx
	subq	%rbp, %rax
	movl	%ecx, 40(%r15)
	cmpq	$2000, %rax
	jg	.L13
	movq	8(%rsp), %rax
	addq	$4, %r14
	addq	$6000, %r13
	addq	$6000, %r12
	cmpq	%rax, %r14
	jne	.L12
	movq	120(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L34
	addq	$136, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L13:
	.cfi_restore_state
	movl	24(%rsp), %eax
	movl	$100, %edi
	movl	%eax, 40(%r15)
	call	usleep@PLT
	subl	$1, 28(%rsp)
	jne	.L18
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
	movq	120(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L34
	movq	stdout(%rip), %rdi
	addq	$136, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	jmp	fflush@PLT
.L36:
	.cfi_restore_state
	movl	$0, 24(%rsp)
	movslq	%esi, %r8
	jmp	.L4
.L34:
	call	__stack_chk_fail@PLT
.L2:
	leaq	__PRETTY_FUNCTION__.1(%rip), %rcx
	movl	$232, %edx
	leaq	.LC0(%rip), %rsi
	leaq	.LC1(%rip), %rdi
	call	__assert_fail@PLT
	.cfi_endproc
.LFE71:
	.size	dshotSendFrames, .-dshotSendFrames
	.p2align 4
	.type	dshotRepeatSendCommand, @function
dshotRepeatSendCommand:
.LFB74:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movl	%ecx, %eax
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	movq	%rdi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movl	%esi, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movslq	%r8d, %rbx
	subq	$144, %rsp
	.cfi_def_cfa_offset 192
	movq	%fs:40, %rcx
	movq	%rcx, 136(%rsp)
	xorl	%ecx, %ecx
	leaq	16(%rsp), %r12
	testl	%esi, %esi
	jle	.L38
	leal	(%rdx,%rdx), %ecx
	leaq	16(%rsp), %r12
	movslq	%esi, %rsi
	orl	%eax, %ecx
	movl	%ecx, %edx
	movl	%ecx, %eax
	sarl	$8, %eax
	sarl	$4, %edx
	xorl	%eax, %edx
	movq	%r12, %rax
	xorl	%ecx, %edx
	sall	$4, %ecx
	andl	$15, %edx
	orl	%ecx, %edx
	leaq	(%r12,%rsi,4), %rcx
	andl	$1, %esi
	je	.L39
	leaq	20(%rsp), %rax
	movl	%edx, (%r12)
	cmpq	%rcx, %rax
	je	.L38
	.p2align 4,,10
	.p2align 3
.L39:
	movl	%edx, (%rax)
	addq	$8, %rax
	movl	%edx, -4(%rax)
	cmpq	%rcx, %rax
	jne	.L39
.L38:
	imulq	$1000000, %rbx, %rbx
	movq	%rsp, %r14
	movl	$4, %edi
	movq	%r14, %rsi
	call	clock_gettime@PLT
	imulq	$1000000000, (%rsp), %rax
	addq	8(%rsp), %rax
	addq	%rax, %rbx
	.p2align 4,,10
	.p2align 3
.L41:
	movq	%r12, %rdx
	movl	%ebp, %esi
	movq	%r13, %rdi
	call	dshotSendFrames
	movl	$100, %edi
	call	usleep@PLT
	movq	%r14, %rsi
	movl	$4, %edi
	call	clock_gettime@PLT
	imulq	$1000000000, (%rsp), %rax
	addq	8(%rsp), %rax
	cmpq	%rbx, %rax
	jb	.L41
	movq	136(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L52
	addq	$144, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L52:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE74:
	.size	dshotRepeatSendCommand, .-dshotRepeatSendCommand
	.p2align 4
	.globl	motorImplementationSet3dModeAndSpinDirection
	.type	motorImplementationSet3dModeAndSpinDirection, @function
motorImplementationSet3dModeAndSpinDirection:
.LFB75:
	.cfi_startproc
	endbr64
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	movl	$25, %r8d
	movl	%ecx, %r12d
	movl	$1, %ecx
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movl	%esi, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movq	%rdi, %rbx
	movl	%edx, dshot3dMode(%rip)
	testl	%edx, %edx
	je	.L54
	movl	$10, %edx
	call	dshotRepeatSendCommand
.L55:
	movl	$25, %r8d
	xorl	%ecx, %ecx
	movl	$12, %edx
	movl	%ebp, %esi
	movq	%rbx, %rdi
	call	dshotRepeatSendCommand
	movl	$25, %r8d
	movl	$1, %ecx
	testl	%r12d, %r12d
	je	.L56
	movl	$21, %edx
	movl	%ebp, %esi
	movq	%rbx, %rdi
	call	dshotRepeatSendCommand
.L57:
	movl	%ebp, %esi
	movq	%rbx, %rdi
	movl	$25, %r8d
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	xorl	%ecx, %ecx
	popq	%rbp
	.cfi_def_cfa_offset 16
	movl	$12, %edx
	popq	%r12
	.cfi_def_cfa_offset 8
	jmp	dshotRepeatSendCommand
	.p2align 4,,10
	.p2align 3
.L54:
	.cfi_restore_state
	movl	$9, %edx
	call	dshotRepeatSendCommand
	jmp	.L55
	.p2align 4,,10
	.p2align 3
.L56:
	movl	$20, %edx
	movl	%ebp, %esi
	movq	%rbx, %rdi
	call	dshotRepeatSendCommand
	jmp	.L57
	.cfi_endproc
.LFE75:
	.size	motorImplementationSet3dModeAndSpinDirection, .-motorImplementationSet3dModeAndSpinDirection
	.p2align 4
	.globl	motorImplementationBeep
	.type	motorImplementationBeep, @function
motorImplementationBeep:
.LFB76:
	.cfi_startproc
	endbr64
	addl	$1, %edx
	xorl	%r8d, %r8d
	xorl	%ecx, %ecx
	jmp	dshotRepeatSendCommand
	.cfi_endproc
.LFE76:
	.size	motorImplementationBeep, .-motorImplementationBeep
	.section	.rodata.str1.1
.LC3:
	.string	"rb"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC4:
	.string	"/proc/device-tree/system/linux,revision"
	.section	.rodata.str1.1
.LC5:
	.string	"debug Error: Can't open '%s'\n"
	.section	.rodata.str1.8
	.align 8
.LC6:
	.string	"debug Error: Revision data too short"
	.align 8
.LC7:
	.string	"debug Error: Unrecognised revision code"
	.section	.rodata.str1.1
.LC8:
	.string	"/dev/mem"
	.section	.rodata.str1.8
	.align 8
.LC9:
	.string	"debug Error: Can't open /dev/mem, run using sudo! "
	.section	.rodata.str1.1
.LC10:
	.string	"debug Mmap error %p\n"
	.text
	.p2align 4
	.globl	motorImplementationInitialize
	.type	motorImplementationInitialize, @function
motorImplementationInitialize:
.LFB77:
	.cfi_startproc
	endbr64
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	leaq	.LC4(%rip), %r13
	leaq	.LC3(%rip), %rsi
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	movq	%rdi, %rbp
	movq	%r13, %rdi
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movl	%edx, %ebx
	subq	$24, %rsp
	.cfi_def_cfa_offset 64
	movq	%fs:40, %rax
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	movl	$0, 4(%rsp)
	call	fopen@PLT
	testq	%rax, %rax
	je	.L80
	leaq	4(%rsp), %rdi
	movq	%rax, %rcx
	movl	$4, %edx
	movq	%rax, %r12
	movl	$1, %esi
	call	fread@PLT
	cmpq	$4, %rax
	jne	.L63
	movzbl	6(%rsp), %r13d
	movq	%r12, %rdi
	shrb	$4, %r13b
	call	fclose@PLT
	cmpb	$2, %r13b
	ja	.L64
	cmpb	$1, %r13b
	sbbl	%r13d, %r13d
	andl	$-520093696, %r13d
	addl	$1059061760, %r13d
.L65:
	movl	$1052674, %esi
	leaq	.LC8(%rip), %rdi
	xorl	%eax, %eax
	call	open@PLT
	movl	%eax, %r12d
	testl	%eax, %eax
	js	.L81
	movslq	%r13d, %r9
	movl	%eax, %r8d
	movl	$1, %ecx
	movl	$3, %edx
	movl	$4096, %esi
	xorl	%edi, %edi
	call	mmap@PLT
	movl	%r12d, %edi
	movq	%rax, dshotGpioMap(%rip)
	call	close@PLT
	movq	dshotGpioMap(%rip), %rdi
	cmpq	$-1, %rdi
	je	.L82
	movq	%rdi, dshotGpio(%rip)
	testl	%ebx, %ebx
	jle	.L72
	movslq	%ebx, %rax
	movq	%rbp, %rsi
	movl	$7, %r9d
	movl	$1, %r8d
	leaq	0(%rbp,%rax,4), %r10
	.p2align 4,,10
	.p2align 3
.L71:
	movslq	(%rsi), %rax
	addq	$4, %rsi
	movq	%rax, %rdx
	imulq	$1717986919, %rax, %rax
	movl	%edx, %ecx
	sarl	$31, %ecx
	sarq	$34, %rax
	subl	%ecx, %eax
	movslq	%eax, %rcx
	leal	(%rax,%rax,4), %eax
	leaq	(%rdi,%rcx,4), %r11
	addl	%eax, %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%r9d, %eax
	movl	(%r11), %r12d
	leal	(%rcx,%rcx,2), %ecx
	sall	%cl, %eax
	notl	%eax
	andl	%r12d, %eax
	movl	%eax, (%r11)
	movl	(%r11), %eax
	btsl	%ecx, %eax
	movl	%edx, %ecx
	movl	%eax, (%r11)
	movl	%r8d, %eax
	sall	%cl, %eax
	movl	%eax, 40(%rdi)
	cmpq	%r10, %rsi
	jne	.L71
.L72:
	movq	8(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L83
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movl	%ebx, %esi
	movq	%rbp, %rdi
	movl	$5000, %r8d
	popq	%rbx
	.cfi_def_cfa_offset 32
	xorl	%ecx, %ecx
	popq	%rbp
	.cfi_def_cfa_offset 24
	xorl	%edx, %edx
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	jmp	dshotRepeatSendCommand
	.p2align 4,,10
	.p2align 3
.L63:
	.cfi_restore_state
	leaq	.LC6(%rip), %rdi
	call	puts@PLT
	movq	%r12, %rdi
	call	fclose@PLT
.L62:
	leaq	.LC7(%rip), %rdi
	call	puts@PLT
.L66:
	movl	$-31457280, %r13d
	jmp	.L65
	.p2align 4,,10
	.p2align 3
.L64:
	cmpb	$3, %r13b
	je	.L66
	jmp	.L62
.L83:
	call	__stack_chk_fail@PLT
.L80:
	movq	%r13, %rdx
	leaq	.LC5(%rip), %rsi
	movl	$2, %edi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	jmp	.L62
.L81:
	leaq	.LC9(%rip), %rdi
	call	puts@PLT
	orl	$-1, %edi
	call	exit@PLT
.L82:
	movl	$2, %edi
	orq	$-1, %rdx
	leaq	.LC10(%rip), %rsi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	orl	$-1, %edi
	call	exit@PLT
	.cfi_endproc
.LFE77:
	.size	motorImplementationInitialize, .-motorImplementationInitialize
	.p2align 4
	.globl	motorImplementationFinalize
	.type	motorImplementationFinalize, @function
motorImplementationFinalize:
.LFB78:
	.cfi_startproc
	endbr64
	movq	dshotGpioMap(%rip), %rdi
	movl	$4096, %esi
	jmp	munmap@PLT
	.cfi_endproc
.LFE78:
	.size	motorImplementationFinalize, .-motorImplementationFinalize
	.p2align 4
	.globl	motorImplementationSendThrottles
	.type	motorImplementationSendThrottles, @function
motorImplementationSendThrottles:
.LFB79:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$136, %rsp
	.cfi_def_cfa_offset 160
	movq	%fs:40, %rax
	movq	%rax, 120(%rsp)
	xorl	%eax, %eax
	cmpl	$26, %esi
	jg	.L86
	movq	%rdi, %r8
	movl	%esi, %r9d
	testl	%esi, %esi
	jle	.L102
	movq	%rdx, %rcx
	movslq	%esi, %rdi
	movl	dshot3dMode(%rip), %r10d
	movsd	.LC16(%rip), %xmm3
	movsd	.LC15(%rip), %xmm1
	movsd	.LC12(%rip), %xmm4
	xorl	%edx, %edx
	movq	%rsp, %rsi
	movsd	.LC13(%rip), %xmm6
	pxor	%xmm2, %xmm2
	jmp	.L93
	.p2align 4,,10
	.p2align 3
.L103:
	comisd	%xmm2, %xmm0
	mulsd	%xmm4, %xmm0
	jb	.L100
	addsd	%xmm6, %xmm0
	cvttsd2sil	%xmm0, %r11d
.L91:
	leal	-48(%r11), %ebx
	xorl	%eax, %eax
	cmpl	$1999, %ebx
	ja	.L92
	leal	(%r11,%r11), %ebx
	sall	$5, %r11d
	movl	%ebx, %eax
	movl	%ebx, %ebp
	sarl	$4, %eax
	sarl	$8, %ebp
	xorl	%ebp, %eax
	xorl	%ebx, %eax
	andl	$15, %eax
	orl	%r11d, %eax
.L92:
	movl	%eax, (%rsi,%rdx,4)
	addq	$1, %rdx
	cmpq	%rdi, %rdx
	je	.L94
.L93:
	movsd	(%rcx,%rdx,8), %xmm0
	testl	%r10d, %r10d
	jne	.L103
	mulsd	%xmm3, %xmm0
	addsd	%xmm1, %xmm0
	cvttsd2sil	%xmm0, %r11d
	jmp	.L91
	.p2align 4,,10
	.p2align 3
.L100:
	movapd	%xmm1, %xmm5
	subsd	%xmm0, %xmm5
	cvttsd2sil	%xmm5, %r11d
	jmp	.L91
.L102:
	movq	%rsp, %rsi
	.p2align 4,,10
	.p2align 3
.L94:
	movq	%rsi, %rdx
	movq	%r8, %rdi
	movl	%r9d, %esi
	call	dshotSendFrames
	movq	120(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L104
	addq	$136, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L104:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
.L86:
	leaq	__PRETTY_FUNCTION__.0(%rip), %rcx
	movl	$391, %edx
	leaq	.LC0(%rip), %rsi
	leaq	.LC1(%rip), %rdi
	call	__assert_fail@PLT
	.cfi_endproc
.LFE79:
	.size	motorImplementationSendThrottles, .-motorImplementationSendThrottles
	.section	.rodata
	.align 32
	.type	__PRETTY_FUNCTION__.0, @object
	.size	__PRETTY_FUNCTION__.0, 33
__PRETTY_FUNCTION__.0:
	.string	"motorImplementationSendThrottles"
	.align 16
	.type	__PRETTY_FUNCTION__.1, @object
	.size	__PRETTY_FUNCTION__.1, 16
__PRETTY_FUNCTION__.1:
	.string	"dshotSendFrames"
	.local	dshot3dMode
	.comm	dshot3dMode,4,4
	.local	dshotGpio
	.comm	dshotGpio,8,8
	.local	dshotGpioMap
	.comm	dshotGpioMap,8,8
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC12:
	.long	0
	.long	1083127808
	.align 8
.LC13:
	.long	0
	.long	1083203584
	.align 8
.LC15:
	.long	0
	.long	1078460416
	.align 8
.LC16:
	.long	0
	.long	1084177408
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
