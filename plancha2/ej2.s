#
# Ejercicio 2 - Plancha 2 (Assembler de x86_64)
#
# Encontrar cuantos bits en uno tiene un entero de 64 bits(quad)
# almacenado en el registro rax
#
.data
test:
      .quad 0x0108

.text
.global main
main:
    movq test, %rbx
    movq $64, %rcx
    xorq %rax, %rax
    contar_uno:
        rolq %rbx
        adcq $0, %rax
        loop contar_uno
        ret

