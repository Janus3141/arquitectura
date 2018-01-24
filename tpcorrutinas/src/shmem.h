
/********** Shared memory header **********/

#ifndef __SHMEM_H
#define __SHMEM_H


/*** Tipos ***/

typedef struct {
    unsigned char pointer;
    int index;
} shmem_t;


typedef struct {
    unsigned char writen;
    int connections;
    int alias;
    char *region;
} shmem_reg_t;


/*** Macros ***/

#define SHMEM_ARR_LEAP 5
#define SHMEM_REGION_SIZE 255

#define SHMEM_INVALID (-1)
#define SHMEM_OK (-2)
#define SHMEM_CLOSED (-3)

#define SEEK_ABS 0
#define SEEK_OFFSET 1

#define SHMEM_REG_NEW 0
#define SHMEM_REG_DISC 1
#define SHMEM_DESTROY_ALL 2


/*** Prototipos y otras definiciones ***/

/* shmem_t shmem_new(char name) */
#define shmem_new(s) (shmem_region_manager(SHMEM_REG_NEW,(s),NULL))

/* void shmem_close(shmem_t *sm) */
#define shmem_close(sm) (shmem_region_manager(SHMEM_REG_DISC,0,(sm))

/* void shmem_destroy(void) */
#define shmem_destroy() (shmem_region_manager(SHMEM_DESTROY_ALL,0,NULL))

shmem_t shmem_region_manager(char, int, shmem_t *);

int shmem_write(shmem_t *, char *, unsigned char);

int shmem_read(shmem_t *, char *, unsigned char);

unsigned char shmem_size(shmem_t *);

int shmem_seek(shmem_t *, int, char);

int shmem_poll(shmem_t *);

int shmem_block_read(shmem_t *, char *, unsigned char);


#endif // __SHMEM_H

