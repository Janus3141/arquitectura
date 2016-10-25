.data
    decfmt: .asciz "%ld\n"
    hexfmt: .asciz "%lx\n"
    i: .quad 0xdeadbeef

.text
.global main
main:
    # El valor del registro rsp
    movq $decfmt, %rdi
    movq %rsp, %rsi
    xorq %rax, %rax
    call printf

    #La dirección de la cadena de formato
    movq $decfmt, %rdi
    movq $decfmt, %rsi
    xorq %rax, %rax
    call printf

    #La dirección de la cadena de formato en hexadecimal
    movq $hexfmt, %rdi
    movq $decfmt, %rsi
    xorq %rax, %rax
    call printf

    #El quad en el tope de la pila
    movq $decfmt, %rdi
    movq (%rsp), %rsi
    xorq %rax, %rax
    call printf

    #El quad ubicado en la dirección rsp+8
    movq $decfmt, %rdi
    movq 8(%rsp), %rsi
    xorq %rax, %rax
    call printf

    #El valor i
    movq $hexfmt, %rdi
    movq i, %rsi
    xorq %rax, %rax
    call printf

    #La dirección de i
    movq $decfmt, %rdi
    movq $i, %rsi
    xorq %rax, %rax
    call printf

    ret
