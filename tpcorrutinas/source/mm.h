
/********** Memory management header **********/

#ifndef __MM_H
#define __MM_H



/********** Memory manager **********/

#define MEM_TASK_SIZE 2000 /* 1KB por Task */
#define MEM_TASKS_PER_SEG 10 /* 10 task por cada segmento de stack */

#define MM_TAKE 1
#define MM_RELEASE 0
#define MM_DESTROY 2

/* void *take_stack(void); */
#define take_stack() memory_manager(NULL,MM_TAKE)

/* void release_stack(Task *); */
#define release_stack(t) memory_manager((t)->mem_position,MM_RELEASE)

/* void destroy_stack(void) */
#define destroy_stack() memory_manager(NULL,MM_DESTROY)

void *memory_manager(void *, char);



/********** Shared memory **********/

typedef struct {
    void *region;
    unsigned char writen;
} shmem_reg_t;

typedef struct {
    int index;
    unsigned char pointer;
} shmem_t;

#define SHMEM_ARR_LEAP 5
#define SHMEM_REGION_SIZE 254

#define SHMEM_INVALID (-1)
#define SHMEM_OK (-2)
#define SHMEM_CLOSED (-3)

#define SEEK_ABS 0
#define SEEK_OFFSET 1
#define SEEK_REV_OFFSET 2

#define SHMEM_REG_NEW 0
#define SHMEM_REG_DEL 1
#define SHMEM_DESTROY_ALL 2

/* shmem_t shmem_new(char name) */
#define shmem_new(s) (shmem_region_manager(SHMEM_REG_NEW,(s),0))

/* void shmem_close(shmem_t *sm) */
#define shmem_close(sm) (shmem_region_manager(SHMEM_REG_DEL,0,(sm)->index))

/* void shmem_destroy(void) */
#define shmem_destroy() (shmem_region_manager(SHMEM_DESTROY_ALL,0,0))

shmem_t shmem_region_manager(char, char, char);

int shmem_write(shmem_t *, char *, unsigned char);

int shmem_read(shmem_t *, void *, unsigned char);

int shmem_seek(shmem_t *, unsigned char, char);

int shmem_poll(shmem_t *);

int shmem_block_read(shmem_t *, void *, unsigned char);


#endif // __MM_H

