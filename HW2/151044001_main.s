	.file	"151044001_main.c"
	.section	.rodata
	.align 8
.LC0:
	.string	"Usage: %s -M number -N number -X text\n"
	.align 8
.LC1:
	.string	" -m\tSpecifies the maximum number of sequences file can store\n"
	.align 8
.LC2:
	.string	" -n\tSpecifies the number of random numbers will be produced for a sequence\n"
	.align 8
.LC3:
	.string	" -x\tSpecifies the file to be used as communication tool\n"
.LC4:
	.string	"-N"
.LC5:
	.string	"-n"
.LC6:
	.string	"-M"
.LC7:
	.string	"-m"
.LC8:
	.string	"-X"
.LC9:
	.string	"-x"
	.align 8
.LC10:
	.string	"Please enter values bigger than 0 as arguments\n"
	.align 8
.LC11:
	.string	"\nError!\nCommunication file already exists\nAn instance of this program might be already running\n"
	.align 8
.LC12:
	.string	"\nSystem Error!\nCommunication file couldn't created: '%s'\nError message: %s\n"
	.align 8
.LC13:
	.string	"\nSystem Error!\nCommunication file couldn't closed: '%s'\nError message: %s\n"
.LC14:
	.string	"\nError!Fork failed.\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	cmpl	$7, -36(%rbp)
	je	.L2
	movq	-48(%rbp), %rax
	movq	(%rax), %rdx
	movq	stderr(%rip), %rax
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$61, %edx
	movl	$1, %esi
	leaq	.LC1(%rip), %rdi
	call	fwrite@PLT
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$75, %edx
	movl	$1, %esi
	leaq	.LC2(%rip), %rdi
	call	fwrite@PLT
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$56, %edx
	movl	$1, %esi
	leaq	.LC3(%rip), %rdi
	call	fwrite@PLT
	movl	$1, %edi
	call	exit@PLT
.L2:
	movl	$0, -32(%rbp)
	jmp	.L3
.L10:
	movl	-32(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC4(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L4
	movl	-32(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC5(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L5
.L4:
	movl	-32(%rbp), %eax
	cltq
	addq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, -24(%rbp)
	jmp	.L6
.L5:
	movl	-32(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC6(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L7
	movl	-32(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC7(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L8
.L7:
	movl	-32(%rbp), %eax
	cltq
	addq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, -28(%rbp)
	jmp	.L6
.L8:
	movl	-32(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC8(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L9
	movl	-32(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	leaq	.LC9(%rip), %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L6
.L9:
	movl	-32(%rbp), %eax
	cltq
	addq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
.L6:
	addl	$1, -32(%rbp)
.L3:
	movl	-36(%rbp), %eax
	subl	$1, %eax
	cmpl	%eax, -32(%rbp)
	jl	.L10
	cmpl	$0, -24(%rbp)
	jle	.L11
	cmpl	$0, -28(%rbp)
	jg	.L12
.L11:
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$47, %edx
	movl	$1, %esi
	leaq	.LC10(%rip), %rdi
	call	fwrite@PLT
	movl	$1, %edi
	call	exit@PLT
.L12:
	movl	$0, %edi
	call	time@PLT
	movl	%eax, %edi
	call	srand@PLT
	movq	-8(%rbp), %rax
	movl	$511, %edx
	movl	$192, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open@PLT
	movl	%eax, -20(%rbp)
	cmpl	$-1, -20(%rbp)
	jne	.L13
	call	__errno_location@PLT
	movl	(%rax), %eax
	cmpl	$17, %eax
	jne	.L14
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$95, %edx
	movl	$1, %esi
	leaq	.LC11(%rip), %rdi
	call	fwrite@PLT
	jmp	.L15
.L14:
	call	__errno_location@PLT
	movl	(%rax), %eax
	movl	%eax, %edi
	call	strerror@PLT
	movq	%rax, %rcx
	movq	stderr(%rip), %rax
	movq	-8(%rbp), %rdx
	leaq	.LC12(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
.L15:
	movl	$1, %edi
	call	exit@PLT
.L13:
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	movl	%eax, -16(%rbp)
	cmpl	$-1, -16(%rbp)
	jne	.L16
	call	__errno_location@PLT
	movl	(%rax), %eax
	movl	%eax, %edi
	call	strerror@PLT
	movq	%rax, %rcx
	movq	stderr(%rip), %rax
	movq	-8(%rbp), %rdx
	leaq	.LC13(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	$1, %edi
	call	exit@PLT
.L16:
	call	fork@PLT
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	cmpl	$-1, %eax
	je	.L18
	testl	%eax, %eax
	je	.L19
	jmp	.L22
.L18:
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$20, %edx
	movl	$1, %esi
	leaq	.LC14(%rip), %rdi
	call	fwrite@PLT
	movl	$1, %edi
	call	exit@PLT
.L19:
	movl	-24(%rbp), %edx
	movq	-8(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	ChildFunction
	jmp	.L20
.L22:
	movl	-12(%rbp), %ecx
	movl	-24(%rbp), %edx
	movl	-28(%rbp), %esi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	ParentFunction
	nop
.L20:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.globl	ParentFunction
	.type	ParentFunction, @function
ParentFunction:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	%edx, -32(%rbp)
	movl	%ecx, -36(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	$0, -16(%rbp)
	leaq	-16(%rbp), %rdx
	movl	-32(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	ProduceSequence@PLT
	movq	-16(%rbp), %rdx
	movl	-28(%rbp), %ecx
	movl	-32(%rbp), %esi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	WriteToFile@PLT
	nop
	movq	-8(%rbp), %rax
	xorq	%fs:40, %rax
	je	.L24
	call	__stack_chk_fail@PLT
.L24:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	ParentFunction, .-ParentFunction
	.section	.rodata
.LC15:
	.string	"Process B: Ehehedha"
	.text
	.globl	ChildFunction
	.type	ChildFunction, @function
ChildFunction:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	leaq	.LC15(%rip), %rdi
	call	puts@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	ChildFunction, .-ChildFunction
	.ident	"GCC: (Ubuntu 7.2.0-8ubuntu3.2) 7.2.0"
	.section	.note.GNU-stack,"",@progbits
