
#ifndef __MM_H
#define __MM_H


/****** Macros *******/

#define MEM_TASK_SIZE 1024 /* 1KB por Task */
#define MEM_TASKS_PER_SEG 10 /* 10 task por cada segmento de stack */

/* void *take_stack(void); */
#define take_stack() memory_manager(NULL,1)

/* void release_stack(Task *); */
#define release_stack(t) memory_manager((t)->mem_position,0)


/****** Prototipos ******/

void *memory_manager(const void *, char);


#endif // __MM_H

