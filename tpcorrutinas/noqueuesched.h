#ifndef __SCHED_H
#define __SCHED_H

#include <setjmp.h>



/********** Definiciones de tipos **********/

/**** Tipos para tareas ****/
typedef enum {ACTIVE, READY, BLOCKED, ZOMBIE} _state;


typedef struct _Task {
    jmp_buf *buf;
    short level : 3;
    _state st;
    void *res; // Resultado de la funcion
    void *mem_start;
    short stack_pos;
    struct _Task *next;
    struct _Task *prev;
} Task;


typedef void *(*TaskFunc)(void *);


/**** Tipos para memoria ****/
typedef enum {FREE, IN_USE} _mem_state;


typedef struct {
    short arr_pos;
    void *mem_pos;
} _mem_info;


/**** Tipos para lock ****/
typedef struct {
    int task_key;
    Task *t;
} _T_lock_pair;


typedef struct {
    int key;
    _T_lock_pair *queue;
    unsigned char q_size;
} Lock;



/********** Definiciones de macros **********/

#define _next(t) ((t)->next)
#define _prev(t) ((t)->prev)
#define _lvl(t) ((t)->level)

#define _TASK_END (SIGRTMIN)
#define _TASK_NEW (SIGRTMIN+1)
#define _TASK_YIELD (SIGRTMIN+2)

#define MS 1000
#define TICK 5
#define QUANTUM 4
#define TIME_L(x) (QUANTUM*TICK*MS*(1<<(x)))

#define _ROUTINE_STACK_SIZE 500000 // 500 kB

#define ACTIVATE(d) (longjmp(*((d)->buf),1))
#define YIELD(o) ( ({ if(setjmp(*((o)->buf))){return;} }) )
// #define FINALIZE(tarea) ( ({ _sched_time_setting={0};  }) )



/********** Prototipos **********/

void take_stack(void);

void create_routine(TaskFunc, void *, Task *);

void stop_routine(Task *);

void *join_routine(Task *,Task *);

void start_sched(Task *);


#endif //__SCHED_H
