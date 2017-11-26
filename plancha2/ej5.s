.global setjmp2
setjmp2:
	movq %r10, (%rdi)
	movq %r13, 8(%rdi)
	movq %r14, 16(%rdi)
	movq %r15, 24(%rdi)
	movq %rbx, 32(%rdi)
	movq %rbp, 40(%rdi)
	movq %rsp, 48(%rdi)
	movq (%rsp), %rax
	movq %rax, 56(%rdi)
	xorq %rax, %rax
	ret

.global longjmp2
longjmp2:
	movq (%rdi), %r10
	movq 8(%rdi), %r13
	movq 16(%rdi), %r14
	movq 24(%rdi), %r15
	movq 32(%rdi), %rbx
	movq 40(%rdi), %rbp
	movq 48(%rdi), %rsp
	movl %esi, %eax
	jnz return
	movq $1, %rax
	return:
		jmp *56(%rdi)
