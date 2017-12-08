
/* Memory management */

#include "scheduler.h"


void *memory_manager(void *init, char action)
{
    constant long MEM_SEG_SIZE = MEM_TASK_SIZE * MEM_TASKS_PER_SEG; 
    /* Posicion inicial del stack, comienzo de scheduler */
    constant void *initial_pos = init;
    /* Posicion mas profunda reservada en el stack */
    static void *deep_pos = NULL; 
    /* Array de 0s y 1s indicando las posiciones reservadas
       (1 si esta reservada) */
    static char *free_spots = NULL;
    /* Primer lugar en el array libre (primer 0) */
    static int next_free_spot = 0;
    /* Segmentos de tamaño MEM_SEG_SIZE reservados hasta el momento */
    static int spots_count = 0;
    if (action == 1) {
        if (next_free_spot >= spots_count) {
            asm("movq %%rsp, %%r12\n"
                "movq %0, %%rsp\n"
                "subq %1, %%rsp\n"
                "movq %rsp, %0\n"
                "movq %%r12, %%rsp"
                : "+m" (deep_pos)
                : "m" (MEM_SEG_SIZE));
            spots_count += MEM_TASKS_PER_SEG;
            free_spots = (char *) realloc(free_spots, spots_count+1);
            for (int i = spots_count - MEM_TASKS_PER_SEG;
                 i < spots_count; i++)
                free_spots[i] = 0;
        }
        free_spots[next_free_spot] = 1;
        void *result = initial_pos + (MEM_TASK_SIZE*next_free_spot);
        for (int i = 0; i < segs_count*MEM_TASKS_PER_SEG; i++) {
            if (free_spots[i] = 0) {
                next_free_spot = i;
                break;
            }
        }
        return result;
    }
    else if (action == 0) {
        long long index = ((long long) init - (long long) initial_pos) / MEM_TASK_SIZE;
        free_spots[index] = 0;
        if (index < next_free_spot)
            next_free_spot = index;
        return NULL;
    }
}

