.text

.global sum_sse

sum_sse:
	movq %rdx, %rcx
	
	addp:
		cmpq $4, %rcx
		jl simple
	
		movaps (%rdi), %xmm0
		movaps (%rsi), %xmm1
		addps %xmm0, %xmm1
		movaps %xmm1, (%rdi) 
	
		addq $16, %rdi
		addq $16, %rsi
		subq $4, %rcx
	
		jmp addp

	simple:
		cmpq $0, %rcx
		je end
		movq %rcx, %rdx
		call sum
	
	end:
		ret
