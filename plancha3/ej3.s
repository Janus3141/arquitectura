.text

.global solve
solve: 
	# Determinante de la matriz
	movss %xmm0, %xmm7
	mulss %xmm3, %xmm7 # ad
	movss %xmm1, %xmm8 
	mulss %xmm2, %xmm8 # bc
	subss %xmm8, %xmm7 # ad - bc

	movq $0, %rax
	cvtsi2ssq %rax, %xmm8
	ucomiss %xmm8, %xmm7
	je zero_determinant 	

	# Determinante de la matriz
    # con la primera columna cambiada
	movss %xmm4, %xmm8
	mulss %xmm3, %xmm8 # ed
	movss %xmm1, %xmm9 
	mulss %xmm5, %xmm9 # bf
	subss %xmm9, %xmm8 # ed - bf

    # Determinante de la matriz
    # con la segunda columna cambiada
	movss %xmm0, %xmm9 
	mulss %xmm5, %xmm9 # af
	movss %xmm4, %xmm10 
	mulss %xmm2, %xmm10 # ec
	subss %xmm10, %xmm9 # af - ec

	# Resultado x
	divss %xmm7, %xmm8 
	movss %xmm8, (%rdi)
	
	# Resultado y
	divss %xmm7, %xmm9 
	movss %xmm9, (%rsi)
	
	movq $0, %rax
	ret
	
	zero_determinant: 
		movq $-1, %rax
		ret 
	
