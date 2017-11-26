#ifndef __SCHED_H
#define __SCHED_H

#include <setjmp.h>



/********** Structures and types definitions **********/

typedef enum {ACTIVE, READY, BLOCKED, ZOMBIE} _state;


typedef struct {
    int key;
    queue qu;
} Lock;


typedef struct _Task {
    jmp_buf *buf;
    short level : 3;
    _state st;
    void *res; // Resultado de la funcion
    void *mem_start;
    short stack_pos;
    struct _Task *next;
} Task;


typedef struct {
    void *result;
    short end : 1; // 1 si la funcion termino antes de llamar a stop
} Fret;


typedef void *(*TaskFunc)(void *);


typedef enum {FREE, IN_USE} _mem_state;


typedef struct {
    short arr_pos;
    void *mem_pos;
} _mem_info;



/********** Macros definitions **********/

#define _next(t) (t->next)

#define TASK_NEW (SIGRTMIN)
#define TASK_END (SIGRTMIN+1)
#define TASK_YIELD (SIGRTMIN+2)

#define MS 1000
#define TICK 5
#define QUANTUM 4
#define TIME_L(x) (QUANTUM*TICK*MS*(1<<(x)))

#define _ROUTINE_STACK_SIZE 500000 // 500 kB

#define ACTIVATE(d) (longjmp(*(d->buf),1))
#define YIELD(o) ( ({ if(setjmp(*(o->buf))){return;} }) )
// #define FINALIZE(tarea) ( ({ _sched_time_setting={0};  }) )



/********** Sched functions prototypes **********/

void take_stack(void);

void create_routine(TaskFunc, void *, Task *);

Fret stop_routine(Task *);

void *join_routine(Task *,Task *);

void start_sched(Task *);


#endif //__SCHED_H
