#ifndef __SCHED_H
#define __SCHED_H

#include <setjmp.h>
#include <signal.h>


/********** Definiciones de tipos **********/

/**** Tipos para tareas ****/
typedef enum {ACTIVE, READY, BLOCKED, ZOMBIE} Task_State;

typedef void *(*TaskFunc)(void *);

typedef struct {
    jmp_buf *buf;
    Task_State st;
    void *arg; // Argumento pasado a la funcion
    TaskFunc fun; // Funcion que ejecutara la tarea
    void *res; // Resultado de la funcion
    char queued:1; // 1 si esta en cola de tareas listas
    void *mem_position; //Posicion inicial de la tarea en el stack
} Task;


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

#define QUEUE_NUMBER 5 // Cantidad de colas que utilizara el scheduler

#define SIG_TASK_END (SIGRTMIN)
#define SIG_TASK_NEW (SIGRTMIN+1)
#define SIG_TASK_YIELD (SIGRTMIN+2)

#define MS 1000 /* 1 ms = 1000 ns */
#define TICK 5
#define QUANTUM 4
#define TIME_L(x) (QUANTUM*TICK*MS*(1<<(x)))

#define ACTIVATE(d) (longjmp(*((d)->buf),1))
#define YIELD(o) ( ({ if(setjmp(*((o)->buf))){return;} }) )
// #define FINALIZE(tarea) ( ({ _sched_time_setting={0};  }) )



/********** Prototipos **********/

void create_routine(TaskFunc, void *, Task *);

void stop_routine(Task *);

void *join_routine(Task *);

void start_sched(Task *);


#endif //__SCHED_H

