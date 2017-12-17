
/********* Locks header file **********/

#ifndef __LOCKS_H
#define __LOCKS_H

/*** Tipos ***/

typedef int task_mutex_t;

typedef struct {
    short state;
    short value;
} task_mutex_arr_t;

/*** Settings ***/

/* Cantidad de espacio para locks que se reserva cuando no hay mas */
#define TASK_MUTEX_SIZE_LEAP 5

/*** Valores de lock ***/

#define TASK_MUTEX_LOCK 0
#define TASK_MUTEX_UNLOCK 1

/*** Valores de estado ***/

#define TASK_MUTEX_INIT 1
#define TASK_MUTEX_UNINIT 0

/*** Acciones del manager ***/

#define TASK_MUTEX_NEW 0
#define TASK_MUTEX_DEL 1
#define TASK_MUTEX_DESTROY 2

/*** Resultados (flags) ***/

#define TASK_MUTEX_OK (-1)
#define TASK_MUTEX_FAIL (-2)

/*** Funciones auxiliares ***/

/* task_mutex_t task_mutex_init(void) */
#define task_mutex_init() (task_mutex_manager(TASK_MUTEX_NEW,0))

/* void task_mutex_remove(task_mutex_t) */
#define task_mutex_remove(m) (task_mutex_manager(TASK_MUTEX_DEL,(m)))

/* void task_mutex_destroy_all(void) */
#define task_mutex_destroy_all() (task_mutex_manager(TASK_MUTEX_DESTROY,0))

/*** Prototipos ***/

task_mutex_t task_mutex_manager(char,task_mutex_t);

int task_mutex_lock(task_mutex_t);

void task_mutex_unlock(task_mutex_t);

#endif //__LOCKS_H


