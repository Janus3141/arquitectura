#ifndef __SCHED_H
#define __SCHED_H

#include <setjmp.h>
#include <queue.h>



/********** Structures and types definitions **********/

typedef enum {ACTIVE, READY, BLOCKED, ZOMBIE} state;

typedef struct {
    int key;
    queue qu;
} lock;

typedef struct {
    jmp_buf *buf;
    short level : 4;
    state st;
    void *res; // Resultado de la funcion
    void *mem_start;
} task;


typedef struct {
    void *result;
    short end : 1; // 1 si la funcion termino antes de llamar a stop
} fret;

typedef void *(*tareaF)(void *);



/********** Macros definitions **********/

#define TASK_NEW (SIGRTMIN)
#define TASK_YIELD (SIGRTMIN+1)
#define START_SCHED (SIGRTMIN+2)

#define MS 1000
#define TICK 5
#define QUANTUM 4
#define TIME_L(x) (QUANTUM*TICK*MS*(1<<(x)))

#define TPILA 4096

#define ACTIVATE(d) (longjmp(*(d->buf),1))
#define YIELD(o) ( ({ if(setjmp(*(o->buf))){return;} }) )
#define FINALIZE(tarea) ( ({ free(tarea->buf);tarea->buf=NULL }) )



/********** Sched functions prototypes **********/

void take_stack(void);

void create_routine(tareaF, void *, task *);

fret stop_routine(task *);

void *join_routine(task *);

void start_sched(task *);


#endif //__SCHED_H
