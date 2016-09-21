.data
	i: .long 0 
	fmt: .asciz "%d\n"
.text
	calculo_muy_complicado:
		movl i, %eax
		imull $2, %eax
		movl %eax, i
		jmp sigue
	
	.global main
	main:
		movq $1, i
		l:
			cmpl $5, i
			jge fin
			jmp calculo_muy_complicado
			sigue:
				incl i
			jmp l

	fin:
		movl i, %eax
		ret
