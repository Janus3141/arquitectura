.text

.global solve
solve:
	movss %xmm3, %xmm7
	movss %xmm2, %xmm6
	mulss %xmm0, %xmm7	# ad
	mulss %xmm1, %xmm6	# bc
	subss %xmm7, %xmm6	# ad - bc (Determinante)
    movq $0, %rax
    cvtsi2ssq %rax, %xmm7
    ucomiss %xmm7, %xmm6
    je zero_determinant

	mulss %xmm4, %xmm3	# ed
	mulss %xmm5, %xmm1	# bf
	subss %xmm3, %xmm1	# ed - bf
	movss %xmm6, %xmm7	# Salvar determinante
	divss %xmm1, %xmm6	# (ed - bf)/(ad - bc) = x
	
	mulss %xmm5, %xmm0	# af
	mulss %xmm4, %xmm2	# ec
	subss %xmm0, %xmm2	# af - ec
	divss %xmm1, %xmm7	# (af - ec)/(ad - bc) = y

	movss %xmm6, (%rdi)
	movss %xmm7, (%rsi)
	ret

	zero_determinant:
		movq $-1, %rax
		ret
