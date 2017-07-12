#ifndef __GUINDOWS2_H
#define __GUINDOWS2_H

#include <setjmp.h>


typedef enum {ACTIVE, READY, BLOCKED, ZOMBIE} state;


typedef struct _task {
    jmp_buf *buf;
    short level : 4;
    state st;
    void *res; // Resultado de la funcion
    void *mem_start;
    int msec;
} task;


typedef struct {
    void *result;
    short end : 1; // 1 si la funcion termino antes de llamar a stop
} fret;


#define TASK_NEW (SIGRTMIN)
#define TASK_YIELD (SIGRTMIN+1)
#define TASK_KILL (SIGRTMIN+2)

#define MS 1000
#define TICK 5
#define QUANTUM 4
#define TIME_L(x) (QUANTUM*TICK*MS*(1<<(x)))

// #define TPILA "4096"
// #define CREATE_STACK(t, f) do{if(setjmp(t)) f(); asm("subq $"TPILA", %rsp");}while(0)
// #define DESTROY_STACK(t) asm("addq $"TPILA", %rsp")

#define ACTIVATE(d) longjmp(*(d->buf),1);
#define YIELD(o) if(setjmp(*(o->buf))){return;}
#define FINALIZE(r) sigqueue(getpid(),TASK_KILL,r);


void error(char *);

task *start_routine(void *(*f)(void *), void *arg);

fret stop_routine(task *);

void kill_routine(task *)

void start_sched(task *);

void sched(int, siginfo_t *, void *);


#endif//__GUINDOWS2_H

// #define YIELD(o) TRANSFER(o, tsched)
// #define ACTIVATE(d) TRANSFER(tsched, d)
