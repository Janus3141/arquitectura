
/********** Scheduler header *********/

#ifndef __SCHED_H
#define __SCHED_H

#include <setjmp.h>
#include <signal.h>


/********** Definiciones de tipos **********/

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



/********** Macros **********/

/* Cantidad de colas que utilizara el scheduler */
#define QUEUE_NUMBER 5 

/* Señales que maneja el scheduler */
#define SIG_TASK_END (SIGRTMIN) // 34
#define SIG_TASK_NEW (SIGRTMIN+1) // 35
#define SIG_TASK_YIELD (SIGRTMIN+2) // 36

#define MS 1000000 /* 1 ms = 1000000 ns */
#define TICK 5
#define QUANTUM 4
/* Tiempo de procesador que obtendra la tarea en funcion de su nivel */
#define TIME_L(x) (QUANTUM*TICK*MS*(1<<(x)))

/* Macros para controlar apropiacion */
#define ACTIVATE(d) (longjmp(*((d)->buf),1))
#define YIELD(o) ( ({ if(setjmp(*((o)->buf))){return;} }) )
#define FINALIZE(t) (scheduler(SIG_TASK_END,NULL,NULL))

/* Abstraccion del bloqueador de scheduler */
#define block_sched() (sched_blocker(0))
#define unblock_sched() (sched_blocker(1))



/********** Prototipos **********/

void create_task(TaskFunc, void *, Task *);

void stop_task(Task *);

void *join_task(Task *);

void start_sched(Task *);

void scheduler(int, siginfo_t *, void *);


#endif //__SCHED_H

