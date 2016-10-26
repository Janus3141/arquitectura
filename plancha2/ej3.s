.data

.text
    .global findstr
    findstr:
        movq %rdx, %rcx
        cld
        movq (%rsi), %rax
        repne scasb
        je success
        movq $-1, %rax
        ret
        success:
            subq %rcx, %rdx
            movq %rdx, %rax
            ret

    .global cmpstr
    cmpstr:
        movq %rdx, %rcx
        cld
        repe cmpsb
        jne fail
        movq $1, %rax
        ret
        fail:
            movq $0, %rax
            ret

    .global fuerzabruta
    fuerzabruta:
        movq %rsi, %r8      # Guardo puntero a segundo argumento        
        movq %rcx, %r9      # Guardo long de segundo argumento
        movq %rdi, %r10     # Guardo puntero a primer argumento
        subq %rcx, %rdx
        incq %rdx 
        movq %rdx, %r12     # r12 = rdx - rcx + 1
        xorq %r13, %r13
        find:
            call findstr
            cmpq $-1, %rax
            je failed
        compare:
            addq %rax, %r13     # r13: Indice a retornar
            subq %rax, %r12
            jl failed
            movq %r9, %rdx
            decq %rdi
            call cmpstr
            cmpq $1, %rax
            je succeed
            movq %r12, %rdx
            movq %r10, %rdi
            addq %r13, %rdi
            movq %r8, %rsi
            jmp find
        succeed:
            movq %r13, %rax
            ret
        failed:
            movq $-1, %rax
            ret
