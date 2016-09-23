.data

.text
    .global findstr
    findstr:
        movq %rdx, %rcx
        cld
        lodsb
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
        movq %rcx, %r9 # Guardo long de segundo argumento
        subq %rcx, %rdx
        incq %rdx # rdx = rdx - rcx + 1
        movq %rdx, %r13
        movq %rsi, %r8 # Guardo puntero a segundo argumento
        movq %rdi, %r10 # Guardo puntero a primer argumento
        mainloop:
            call findstr
            cmpq $-1, %rax
            jne compare
        compare:
            movq %rdx, %r12 # Indice a retornar donde comienza la subcadena buscada
            movq %r9, %rdx
            movq %r8, %rsi
            call cmpstr
            cmpq $1, %rax
            je success
            subq %r12, %r13
            jz fail
            movq %r13, %rdx
            jmp mainloop
        succeed:
            movq %r12, %rax
            ret
        failed:
            movq $-1, %rax
            ret
