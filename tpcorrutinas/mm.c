
/* Memory management */

#include "mm.h"
#include "scheduler.h"
#include <stdlib.h>
#include <malloc.h>



/****** Reserva de stack para tareas ******/

/* Posicion inicial del stack, comienzo de scheduler */
extern void *initial_rsp;

void *memory_manager(void *set_free, char action)
{
    const long MEM_SEG_SIZE = MEM_TASK_SIZE * MEM_TASKS_PER_SEG; 
    /* Array de 0s y 1s indicando las posiciones reservadas
       (1 si esta reservada) */
    static char *free_spots = NULL;
    /* Primer lugar en el array libre (primer 0) */
    static int next_free_spot = 0;
    /* Segmentos de tamaño MEM_SEG_SIZE reservados hasta el momento */
    static int spots_count = 0;
    if (action == 1) {
        if (next_free_spot >= spots_count) {
            spots_count += MEM_TASKS_PER_SEG;
            free_spots = (char *) realloc(free_spots, spots_count+1);
            for (int i = spots_count - MEM_TASKS_PER_SEG;
                 i < spots_count; i++)
                free_spots[i] = 0;
        }
        free_spots[next_free_spot] = 1;
        void *result = initial_rsp + (MEM_TASK_SIZE*next_free_spot);
        for (int i = 0; i <= spots_count; i++) {
            if (free_spots[i] == 0) {
                next_free_spot = i;
                break;
            }
        }
        return result;
    }
    else if (action == 0) {
        long long index = ((long long) set_free - (long long) initial_rsp) / MEM_TASK_SIZE;
        free_spots[index] = 0;
        if (index < next_free_spot)
            next_free_spot = index;
        return NULL;
    }
}



/****** Shared memory ******/
/*

void *mem_regions = NULL;


shmem_t shmem_con(unsigned char n, unsigned char size)
{
    static int reg_arr_size = 0;
    static int reg_next_free = 0;
    if (n == SHMEM_NEW) {
        block_sched();
        if (reg_next_free >= reg_arr_size) {
            reg_arr_size += ARR_SIZE_LEAP;
            mem_regions = realloc(mem_regions, sizeof(void *) * reg_arr_size + 1);
            if (mem_regions == NULL)
                __error("shmem malloc error", 18);
            for (int i = reg_arr_size - ARR_SIZE_LEAP; i <= arr_size; i++)
                mem_regions[i] = NULL;
        }
        void *mem_pos = malloc(size);
        mem_regions[reg_next_free] = mem_pos;
        shmem_t result = {.index = reg_next_free,
                          .size = size};
        for (int i = 0; i <= reg_arr_size; i++) {
            if (mem_regions[i] = NULL)
                reg_next_free = i;
        }
        unblock_sched();
        return result;
    }
    else if (n < reg_arr_size && mem_regions[n] != NULL) {
        free(mem_regions[n]);
        mem_regions[n] = NULL;
        if (n < reg_next_free)
            reg_next_free = n;
        return SHMEM_OK;
    }
}


shmem_
*/
