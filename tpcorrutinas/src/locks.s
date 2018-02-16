
.global task_sp_lock
task_sp_lock:
    movq %rdi, %rbx
    jmp prelock
    yield:
        callq task_yield
    prelock:
        movq (%rbx), %rax
        test %rax, %rax
        je yield
    trylock:
        movl $1, %eax
        movl $0, %ecx
        cmpxchg %rcx, (%rbx)
        jnz yield
    ret


.global task_sp_unlock
task_sp_unlock:
    movq $1, (%rdi)
    ret

