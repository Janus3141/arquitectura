
/* Implementaciones de funciones del scheduler */

// TO DO
/* Comentario sobre codigo */

#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <malloc.h>
#include <setjmp.h>
#include <poll.h>
#include "scheduler.h"
#include "pqueue.h"
#include "gerror.h"
#include "shmem.h"

#include <stdio.h>

#define print_sched() (write(STDOUT_FILENO, "sched\n", 6))


/********** Variables globales **********/

/* Backup de manejo de señales */
struct sigaction old_yield_sig;
struct sigaction old_new_sig;
struct sigaction old_end_sig;

/* Setting por defecto de timers */
timer_t sched_yield_timer,
        sched_lift_timer,
        sched_idletask_timer;
struct itimerspec sched_yield_timespec = {0};
struct itimerspec sched_stop_timer = {0};
struct itimerspec sched_lift_timespec = {0};
struct itimerspec sched_gettime = {0};
struct itimerspec sched_idletask_timespec = {0};

/* Tarea en ejecucion */
q_elem *qelem_current = NULL;

/* Direccion de stack considerada inicio del sched */
void *initial_rsp;

/* Colas de tareas en espera */
pqueue *queues;

/* Array de 0s y 1s indicando las posiciones reservadas
   (1 si esta reservada) de stack para tareas */
char *mm_free_spots = NULL;



/********** Definiciones de funciones **********/


void *memory_manager(void *set_free, char action)
{
    /* Primer lugar en el array libre (primer 0) */
    static int next_free_spot = 0;
    /* Segmentos de tamaño MEM_TASK_SIZE reservados hasta el momento */
    static int spots_count = 0;

    /* Ceder porcion de stack a la tarea */
    if (action == MM_TAKE) {
        /* Comprobar espacio en el array de posiciones reservadas */
        if (next_free_spot >= spots_count) {
            int i = spots_count;
            spots_count += MEM_TASKS_PER_SEG;
            mm_free_spots = realloc(mm_free_spots, sizeof(char)*(spots_count+1));
            if (mm_free_spots == NULL)
                __error("memory_manager malloc error",27);
            for (; i <= spots_count; i++)
                mm_free_spots[i] = 0;
        }
        /* Indicar posicion reservada en el array, calcular posicion en stack
        y setear next_free_spot */
        mm_free_spots[next_free_spot] = 1;
        void *result = initial_rsp - (MEM_TASK_SIZE*next_free_spot);
        for (int i = 0; i <= spots_count; i++) {
            if (mm_free_spots[i] == 0) {
                next_free_spot = i;
                break;
            }
        }
        return result;
    }

    /* Marcar como libre posicion de stack de tarea que termino */
    else if (action == MM_RELEASE) {
        long long index = ((long long) initial_rsp - (long long) set_free) / MEM_TASK_SIZE;
        mm_free_spots[index] = 0;
        if (index < next_free_spot)
            next_free_spot = index;
    }

    /* Liberar toda la memoria reservada (solo destruccion de scheduler) */
    else if (action == MM_DESTROY) {
        free(mm_free_spots);
    }

    return NULL;
}


void create_task(TaskFunc f, void *arg, Task *new)
{
    /* Primero se desarma el timer para ejecutar malloc de forma segura */
    if (timer_settime(sched_yield_timer, 0, &sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
    /* Buffer para volver a este hilo de ejecucion (el del llamante) */
    jmp_buf creator_buf;

    /* Preparacion de la nueva tarea */
    jmp_buf *created_buf = malloc(sizeof(jmp_buf));
    if (created_buf == NULL)
        __error("create_task malloc error",24);
    new -> buf = created_buf;
    new -> st = READY;
    new -> arg = arg;
    new -> fun = f;
    new -> mem_position = take_stack();

    /* setjmp y llamada a _start_task.
       Esa funcion vuelve con longjmp a creator_buf */ 
    if (setjmp(creator_buf) == 0) {
        asm("movq %0, %%rdi\n"
            "movq %1, %%rsi\n"
            "movq %2, %%rsp\n"
            "callq _start_task"
            :
            : "g" (new), "g" (creator_buf), "g" (new->mem_position));
    }
    /* Se informa al scheduler de la nueva tarea,
       y se pone en marcha nuevamente */
    else {    
        union sigval task_pointer = {.sival_ptr = new};
        siginfo_t inf = {.si_value = task_pointer};
        /* En este punto se puede enviar una señal o llamar directamente
           a scheduler, lo cual es mas seguro */
        /*if (sigqueue(getpid(), SIG_TASK_NEW, task_pointer))
            __error("sigqueue error",14);*/
        scheduler(SIG_TASK_NEW, &inf, NULL);
    }
    return;
}



void _start_task(Task *new, jmp_buf *b)
{
    /* Solo para ser llamada por create_task() */

    asm("movq %0, %%r15\n"
        :
        : "g" (new));

    /* Se coloca un checkpoint en new->buf, el scheduler llamara
       a esta funcion volviendo con longjmp a ese buffer. */
    if (setjmp(*(new->buf)) == 0) {
        longjmp(*b,1);
    }
    else {
        Task *renew;
        asm("movq %%r15, %0\n"
            : "+g" (renew));
        renew -> res = (renew->fun)(renew->arg);
        renew -> st = ZOMBIE;
        stop_task(renew);
    }
}


void stop_task(Task *r)
{
    /* Si se quiere saber si la funcion termino antes de llamar a stop,
       verificar state */
    if (timer_settime(sched_yield_timer, 0, &sched_stop_timer, NULL))
        __error("Error disarming timer", 21);
    /* Si la funcion no habia terminado, su estado no sera ZOMBIE,
       mejor poner como resultado NULL */
    if (r->st != ZOMBIE)
        r->res = NULL;
    free(r->buf);
    release_stack(r);
    /* FINALIZE llama al scheduler y le informa que no debe
       reencolar esta tarea */
    FINALIZE(r);
}


void *join_task(Task *to)
{
    /* Espera a que la tarea indicada termine (estado ZOMBIE), haciendo
       un spin en la condicion */
    while (to -> st != ZOMBIE) {
        if (timer_settime(sched_yield_timer, 0, &sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
        scheduler(SIG_TASK_YIELD, NULL, NULL);
    }
    return to->res;
}


void block_task(Task *target)
{
    if (target -> st != ZOMBIE)
        target -> st = BLOCKED;
}


void unblock_task(Task *target)
{
    struct itimerspec old;
    if (timer_settime(sched_yield_timer, 0, &sched_stop_timer, &old))
        __error("Error disarming timer", 21);
    target -> st = READY;
    /* Si la tarea ya no estaba en la cola de espera se la debe
       ingresar al scheduler como una nueva tarea (pero se mantiene
       todo lo que se ejecuto hasta el momento) */
    if (target -> queued == 0) {
        union sigval task_pointer = {.sival_ptr = target};
        siginfo_t data = {.si_value = task_pointer};
        scheduler(SIG_TASK_NEW, &data, NULL);
    }
    /* Si la tarea aun estaba en la cola, basta con cambiar el
       estado de la tarea a READY */
    else {
        old.it_interval = (struct timespec) {0};
        timer_settime(sched_yield_timer, 0, &old, NULL);
    }
}


void scheduler(int signum, siginfo_t *data, void* extra)
{
    static Task *current_task;
    static q_elem *poped;

    // print_sched();

    /* Desencolar proxima tarea a ser ejecutada
       Si la tarea esta bloqueada se libera de la cola,
       y luego sera reinsertada por unblock_task */
    while (1) {
        poped = queue_pop(queues);
        if (poped == NULL)
            break;
        current_task = (Task *) poped -> data;
        current_task -> queued = 0;
        if (current_task -> st == BLOCKED)
            free(poped);
        else
            break;
    }

    /* Guardar tarea anterior y guardar checkpoint */
    if (signum != SIG_TASK_END && qelem_current != NULL) {
        if (qelem_current -> lvl < QUEUE_NUMBER - 1 &&
            signum != SIG_TASK_NEW && data != NULL)
            qelem_current -> lvl ++;
        queue_insert(queues, qelem_current);
        current_task = (Task *) qelem_current -> data;
        current_task -> queued = 1;
        if (current_task -> st == ACTIVE)
            current_task -> st = READY;
        YIELD(current_task);
    }

    /* Encolar nueva tarea en la cola de mayor prioridad */
    if (signum == SIG_TASK_NEW) {
        queue_new_node(queues, data -> si_value.sival_ptr);
        ((Task *) data -> si_value.sival_ptr) -> queued = 1;
    }

    /* Incrementar prioridad de todas las tareas a la maxima cada
       cierto intervalo predeterminado (ver start_sched) */
    if (timer_gettime(sched_lift_timer, &sched_gettime))
        __error("gettime error", 13);
    if (sched_gettime.it_value.tv_sec == 0 &&
        sched_gettime.it_value.tv_nsec == 0) {
        queue_lift(queues);
        if (timer_settime(sched_lift_timer, 0, &sched_lift_timespec, NULL))
            __error("Error setting timer", 19);
    }

    if (poped == NULL) {
        if (timer_settime(sched_idletask_timer, 0, &sched_idletask_timespec, NULL))
            __error("Error setting timer", 19);
        qelem_current = NULL;
        idle_task();
    }
    else {
        qelem_current = poped;
        current_task = (Task *) qelem_current -> data;
    }

    current_task -> st = ACTIVE;
    sched_yield_timespec.it_value.tv_nsec = QUANTUM(qelem_current -> lvl);
    if (timer_settime(sched_yield_timer, 0, &sched_yield_timespec, NULL))
        __error("Error setting timer", 19);
    ACTIVATE(current_task);
}


void start_sched(Task *maintask)
{
	/* Setting de timer, señales y handler */
    /* Señales a bloquear durante la ejecucion de scheduler */
    sigset_t *block_these = malloc(sizeof(sigset_t));;
    if (block_these == NULL)
        __error("start_sched malloc error",24);
    if(sigfillset(block_these) ||
       sigdelset(block_these,SIG_TASK_YIELD) ||
       sigdelset(block_these,SIG_TASK_NEW) ||
       sigdelset(block_these,SIGTSTP) ||
       sigdelset(block_these,SIGINT) ||
       sigdelset(block_these,SIGKILL))
        __error("Error in signal config", 22);
    /* Accion de las señales */
    struct sigaction *sa = malloc(sizeof(struct sigaction));
    sa -> sa_sigaction = scheduler;
    sa -> sa_mask = *block_these;
    sa -> sa_flags = SA_SIGINFO | SA_RESTART | SA_NODEFER;
    if (sigaction(SIG_TASK_YIELD, sa, &old_yield_sig))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_NEW, sa, &old_new_sig))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_END, sa, &old_end_sig))
        __error("Error setting sigaction", 23);
    /* Setting de sched_yield_timer */
    struct sigevent *yield_se = malloc(sizeof(struct sigevent));
    yield_se -> sigev_notify = SIGEV_SIGNAL;
    yield_se -> sigev_signo = SIG_TASK_YIELD;
    if (timer_create(CLOCK_REALTIME, yield_se, &sched_yield_timer))
        __error("Error creating timer", 20);
    /* Setting de sched_lift_timer */
    struct sigevent *lift_se = malloc(sizeof(struct sigevent));
    lift_se -> sigev_notify = SIGEV_NONE;
    if (timer_create(CLOCK_REALTIME, lift_se, &sched_lift_timer))
        __error("Error creating timer", 20);
    if (timer_settime(sched_lift_timer, 0, &sched_stop_timer, NULL))
        __error("Error setting timer", 19);
    sched_lift_timespec.it_value.tv_sec = SCHED_LIFT_SECS;
    sched_lift_timespec.it_value.tv_nsec = SCHED_LIFT_NSECS;
    /* Setting de sched_idletask_timer */
    struct sigevent *idletask_se = malloc(sizeof(struct sigevent));
    idletask_se -> sigev_notify = SIGEV_SIGNAL;
    idletask_se -> sigev_signo = SIG_TASK_YIELD;
    if (timer_create(CLOCK_REALTIME, idletask_se, &sched_idletask_timer))
        __error("Error creating timer", 20);
    sched_idletask_timespec.it_value.tv_sec = 0;
    sched_idletask_timespec.it_value.tv_nsec = SCHED_IDLETASK_NSECS;

    /* Setting de maintask */
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    maintask -> buf = jb;
    maintask -> st = ACTIVE;
    maintask -> arg = NULL;
    maintask -> fun = NULL;
    maintask -> res = NULL;
    maintask -> queued = 0;
    /* initial_rsp se inicia con el rsp actual */
    asm("movq %%rsp, %0"
    	: "+m" (initial_rsp));
    maintask -> mem_position = take_stack();

    /* Creacion de colas para tareas */
    queues = queue_create(QUEUE_NUMBER);

    /* Primer llamado al handler (sched) */
    YIELD(maintask);
    union sigval task_pointer = {.sival_ptr = maintask};
    siginfo_t inf = {.si_value = task_pointer};
    scheduler(SIG_TASK_NEW, &inf, NULL);
}


void sched_blocker(char act) {
    // act = 0 -> block
    // act = 1 -> unblock 
    static struct itimerspec old;
    if (act == 0) {
        if (timer_settime(sched_yield_timer, 0, &sched_stop_timer, &old))
            __error("Error disarming timer",21);
    }
    else if (act == 1) {
        if (timer_settime(sched_yield_timer, 0, &old, NULL))
            __error("Error setting timer",19);
    }
    return;
}


Task *task_current(void)
{
    if (qelem_current != NULL)
        return (Task *)(qelem_current -> data);
}


void task_state(Task_State s)
{
    if (qelem_current != NULL)
        ((Task *) qelem_current -> data) -> st = s;
}


void task_yield(void)
{
    /* Devuelve el procesador al scheduler. No cambia la prioridad
       de la tarea */
    if (timer_settime(sched_yield_timer, 0, &sched_stop_timer, NULL))
        __error("Error disarming timer", 21);
    scheduler(SIG_TASK_YIELD, NULL, NULL);
}


int task_poll(struct pollfd *fds, nfds_t nfds)
{
    int ret_value = poll(fds,nfds,0);
    while(ret_value <= 0) {
        if (ret_value < 0)
            return ret_value;
        else {
            task_yield();
            ret_value = poll(fds,nfds,0);
        }
    }
    return ret_value;
}


void destroy_sched(void)
{
    if (timer_settime(sched_yield_timer, 0, &sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
    queue_destroy(queues);
    if (qelem_current != NULL)
        free(qelem_current);
    destroy_stack();
    shmem_destroy();
    if (sigaction(SIG_TASK_YIELD, &old_yield_sig, NULL))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_NEW, &old_new_sig, NULL))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_END, &old_end_sig, NULL))
        __error("Error setting sigaction", 23);
    return;
}


void idle_task(void) {
    while (1) {}
}



