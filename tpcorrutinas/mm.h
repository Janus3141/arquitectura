
#ifndef __MM_H
#define __MM_H



/********** Memory manager **********/

/****** Macros *******/

#define MEM_TASK_SIZE 2000 /* 1KB por Task */
#define MEM_TASKS_PER_SEG 10 /* 10 task por cada segmento de stack */

/* void *take_stack(void); */
#define take_stack() memory_manager(NULL,1)

/* void release_stack(Task *); */
#define release_stack(t) memory_manager((t)->mem_position,0)

void *memory_manager(void *, char);



/********** Shared memory **********/

typedef struct {
    unsigned char index;
    unsigned char size;
} shmem_t;

#define SHMEM_CON_NEW (-1)
#define SHMEM_CON_ERR (-2)
#define SHMEM_CON_OK  (-3)

/* shmem_t shmem_new(unsigned char size) */
#define shmem_new(s) (shmem_con(SHMEM_CON_NEW,(s)))

/* void shmem_close(shmem_t sm) */
#define shmem_close(sm) (shmem_con((sm).index, 0))

shmem_t shmem_con(unsigned char, unsigned char);


#endif // __MM_H

