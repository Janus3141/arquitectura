.global setjmp2
setjmp2:
    movq %rbx, (%rdi)
    movq %rcx, 8(%rdi)
    movq %rdx, 16(%rdi)
	movq %rsi, 24(%rdi)
	movq %rdi, 32(%rdi)
    movq %rbp, 40(%rdi)
	movq %rsp, 48(%rdi)
	movq %r8, 56(%rdi)
	movq %r9, 64(%rdi)
	movq %r10, 72(%rdi)
	movq %r11, 80(%rdi)
	movq %r12, 88(%rdi)
	movq %r13, 96(%rdi)
	movq %r14, 104(%rdi)
	movq %r15, 112(%rdi)
    sahf
    movq %rax, 120(%rdi)
	movq (%rsp), %rax
	movq %rax, 128(%rdi)
	xorq %rax, %rax
	ret


.global longjmp2
longjmp2:
    movq (%rdi), %rbx
    movq 8(%rdi), %rcx
    movq 16(%rdi), %rdx
	movq 24(%rdi), %rsi
	movq 32(%rdi), %rdi
    movq 40(%rdi), %rbp
	movq 48(%rdi), %rsp
	movq 56(%rdi), %r8
	movq 64(%rdi), %r9
	movq 72(%rdi), %r10
	movq 80(%rdi), %r11
	movq 88(%rdi), %r12
	movq 96(%rdi), %r13
	movq 104(%rdi), %r14
	movq 112(%rdi), %r15
    movq 120(%rdi), %rax
    lahf
	movl %esi, %eax
	jnz return
	movq $1, %rax
	return:
		jmp *128(%rdi)

