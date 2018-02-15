
/********** Scheduler header *********/

#ifndef __SCHED_H
#define __SCHED_H

#include <signal.h>
#include <poll.h>


/********** Definiciones de tipos **********/

typedef enum {ACTIVE, READY, BLOCKED, ZOMBIE} Task_State;

typedef void *(*TaskFunc)(void *);

typedef struct {
    char *context;
    Task_State st;
    void *arg; // Argumento pasado a la funcion
    TaskFunc fun; // Funcion que ejecutara la tarea
    void *res; // Resultado de la funcion
    char queued:1; // 1 si esta en cola de tareas listas
    void *mem_position; //Posicion inicial de la tarea en el stack
} Task;


typedef long long task_sp_t;



/********** Macros **********/

/* Cantidad de colas que utilizara el scheduler */
#define QUEUE_NUMBER 5 

/* Señales que maneja el scheduler */
#define SIG_TASK_END (SIGRTMIN) // 34
#define SIG_TASK_NEW (SIGRTMIN+1) // 35
#define SIG_TASK_YIELD (SIGRTMIN+2) // 36

#define MS 1000000 /* 1 ms = 1000000 ns */
#define TICK 20
/* Tiempo de procesador que obtendra la tarea en funcion de su nivel */
#define QUANTUM(x) (TICK*MS*(1<<(x)))

/* Definen cada cuanto tiempo se levanta la prioridad de todas las tareas */
#define SCHED_LIFT_SECS 0
#define SCHED_LIFT_NSECS (500*MS)

/* Tiempo que dura en ejecucion la tarea 'idle' */
#define SCHED_IDLETASK_NSECS (1*MS)

#define MEM_TASK_SIZE 2000 /* Cantidad de bytes de stack por tarea */
#define MEM_TASKS_PER_SEG 10 /* 10 task por cada segmento de stack */

/* Flags de memory_manager */
#define MM_TAKE 1
#define MM_RELEASE 0
#define MM_DESTROY 2

/* Tamaño del buffer (context) de una tarea.
   Depende de la implementacion en jmp.s */
#define CONTEXT_S 136

/* Macros para controlar apropiacion */
#define ACTIVATE(d) (longjmp2((d)->context,1))
#define YIELD(o) ( ({ if(setjmp2((o)->context)){return;} }) )
#define FINALIZE(t) (scheduler(SIG_TASK_END,NULL,NULL))

/* Inicializador para locks */
#define TASK_SP_INIT 1


/*** Abstracciones del bloqueador de scheduler ***/
#define block_sched() (sched_blocker(0))
#define unblock_sched() (sched_blocker(1))


/*** Abstracciones de memory_manager ***/

/* void *take_stack(void); */
#define take_stack() memory_manager(NULL,MM_TAKE)

/* void release_stack(Task *); */
#define release_stack(t) memory_manager((t)->mem_position,MM_RELEASE)

/* void destroy_stack(void) */
#define destroy_stack() memory_manager(NULL,MM_DESTROY)



/********** Prototipos **********/

int setjmp2(char *);

int longjmp2(char *, int);

void *memory_manager(void *, char);

void create_task(TaskFunc, void *, Task *);

void stop_task(Task *);

void *join_task(Task *);

void block_task(Task *);

void unblock_task(Task *);

int task_poll(struct pollfd *, nfds_t);

void start_sched(Task *);

void task_yield(void);

void sched_blocker(char);

void scheduler(int, siginfo_t *, void *);

void destroy_sched(void);

void idle_task(void);

Task *task_current(void);

void task_state(Task_State);

void task_sp_lock(task_sp_t *);

void task_sp_unlock(task_sp_t *);


#endif //__SCHED_H

