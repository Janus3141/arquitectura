.text

.global sum
sum: 
	movq %rdx, %rcx
	cmpq $0, %rcx
	je end
	add: 
		movss (%rsi), %xmm0
		movss (%rdi), %xmm1
		addss %xmm0, %xmm1
		movss %xmm1, (%rdi)   
		addq $4, %rsi
		addq $4, %rdi
		loop add
	end: 
		ret

