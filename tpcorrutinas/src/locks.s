
.global task_sp_lock
task_sp_lock:
    # Por seguridad se guarda rdi en rbx
    movq %rdi, %rbx
    jmp prelock

    yield:
        callq task_yield

    # Se testea si el lock es 0 (esta tomado)
    # Mejora el rendimiento ya que no escribe como en trylock
    prelock:
        movq (%rbx), %rax
        test %rax, %rax
        je yield

    # Si se leyo que el lock es 1 en prelock,
    # se trata de tomar el lock
    trylock:
        movl $1, %eax
        movl $0, %ecx
        cmpxchg %rcx, (%rbx)
        # Si ZF = 0 no se tomo el lock,
        # entonces se cede el procesador
        jnz yield
    ret


.global task_sp_unlock
task_sp_unlock:
    # Para soltarlo, solo se necesita poner en 1 el lock
    movq $1, (%rdi)
    ret

