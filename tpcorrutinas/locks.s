
# Manager: 0 - Bloquear, 1 - Despertar

# void acquire(lock *l, task *t)
acquire:
    movq $0, %rax
    xchg (%rdi), %rax
    cmpq %rax, $0
    je equal
    ret
    equal:
        movq $0, %rdx
        call manager

# void release(lock *l)
# Castear el puntero en manager
release:
    movq $1, (%rdi)
    movq $1, %rsi
    call manager
