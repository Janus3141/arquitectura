#ifndef __SCHED_H
#define __SCHED_H

#include <setjmp.h>
#include <queue.c>


typedef enum {ACTIVE, READY, BLOCKED, ZOMBIE} state;

typedef struct {
    int key;
    queue qu;
} lock;

typedef struct _task {
    jmp_buf *buf;
    short level : 4; //No lo estamos usando, borrar?
    state st;
    void *res; // Resultado de la funcion
    void *mem_start;
} task;


typedef struct {
    void *result;
    short end : 1; // 1 si la funcion termino antes de llamar a stop
} fret;


#define TASK_NEW (SIGRTMIN)
#define TASK_YIELD (SIGRTMIN+1)

#define MS 1000
#define TICK 5
#define QUANTUM 4
#define TIME_L(x) (QUANTUM*TICK*MS*(1<<(x)))

#define TPILA 4096

#define ACTIVATE(d) (longjmp(*(d->buf),1))
#define YIELD(o) if(setjmp(*(o->buf))){return;}
#define FINALIZE(tarea) (free(tarea->buf);tarea->buf=NULL) //Redef Nisman


typedef void *(*tareaF)(void *);

void error(char *);

void take_stack(void);

task *start_routine(void *(*f)(void *), void *arg);

fret stop_routine(task *);

void kill_routine(task *)

void start_sched(task *);

void sched(int, siginfo_t *, void *);


#endif //__SCHED_H
