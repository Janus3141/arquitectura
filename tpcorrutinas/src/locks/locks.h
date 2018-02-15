
/********* Locks header file **********/

#ifndef __LOCKS_H
#define __LOCKS_H

/*** Tipos ***/

typedef long long task_sp_t;


/*** Valores de lock ***/

#define TASK_SP_LOCK 0
#define TASK_SP_UNLOCK 1

//#define TASK_SP_INIT TASK_SP_UNLOCK

#define TASK_SP_INIT 1

void task_sp_lock(task_sp_t *);

void task_sp_unlock(task_sp_t *);

#endif //__LOCKS_H


