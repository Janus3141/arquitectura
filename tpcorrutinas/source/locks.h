
/********* Locks header file **********/

#ifndef __LOCKS_H
#define __LOCKS_H

/*** Tipos ***/

typedef int task_sp_t;

typedef struct {
    short state;
    short value;
} task_sp_arr_t;

/*** Settings ***/

/* Cantidad de espacio para locks que se reserva cuando no hay mas */
#define TASK_SP_SIZE_LEAP 5

/*** Valores de lock ***/

#define TASK_SP_LOCK 0
#define TASK_SP_UNLOCK 1

/*** Valores de estado ***/

#define TASK_SP_INIT 1
#define TASK_SP_UNINIT 0

/*** Acciones del manager ***/

#define TASK_SP_NEW 0
#define TASK_SP_DEL 1
#define TASK_SP_DESTROY 2

/*** Resultados (flags) ***/

#define TASK_SP_OK (-1)
#define TASK_SP_FAIL (-2)

/*** Funciones auxiliares ***/

/* task_sp_t task_sp_init(void) */
#define task_sp_init() (task_sp_manager(TASK_SP_NEW,0))

/* void task_sp_remove(task_sp_t) */
#define task_sp_remove(m) (task_sp_manager(TASK_SP_DEL,(m)))

/* void task_sp_destroy_all(void) */
#define task_sp_destroy_all() (task_sp_manager(TASK_SP_DESTROY,0))

/*** Prototipos ***/

task_sp_t task_sp_manager(char,task_sp_t);

int task_sp_lock(task_sp_t);

void task_sp_unlock(task_sp_t);

#endif //__LOCKS_H


