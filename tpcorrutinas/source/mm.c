
/********** Memory management **********/

#include "mm.h"
#include "scheduler.h"
#include "gerror.h"
#include <stdlib.h>
#include <malloc.h>



/********** Memory manager **********/
/****** Reserva de stack para tareas ******/

/* Posicion inicial del stack, comienzo de scheduler */
extern void *initial_rsp;

void *memory_manager(void *set_free, char action)
{
	/* Tamaño de un segmento de stack para una tarea */
    const long MEM_SEG_SIZE = MEM_TASK_SIZE * MEM_TASKS_PER_SEG; 
    /* Array de 0s y 1s indicando las posiciones reservadas
       (1 si esta reservada) */
    static char *free_spots = NULL;
    /* Primer lugar en el array libre (primer 0) */
    static int next_free_spot = 0;
    /* Segmentos de tamaño MEM_SEG_SIZE reservados hasta el momento */
    static int spots_count = 0;
    if (action == MM_TAKE) {
        if (next_free_spot >= spots_count) {
            spots_count += MEM_TASKS_PER_SEG;
            free_spots = (char *) realloc(free_spots, spots_count+1);
            if (free_spots == NULL)
            	__error("memory_manager malloc error",27);
            for (int i = spots_count - MEM_TASKS_PER_SEG;
                 i < spots_count; i++)
                free_spots[i] = 0;
        }
        free_spots[next_free_spot] = 1;
        void *result = initial_rsp - (MEM_TASK_SIZE*next_free_spot);
        for (int i = 0; i <= spots_count; i++) {
            if (free_spots[i] == 0) {
                next_free_spot = i;
                break;
            }
        }
        return result;
    }
    else if (action == MM_RELEASE) {
        long long index = ((long long) set_free - (long long) initial_rsp) / MEM_TASK_SIZE;
        free_spots[index] = 0;
        if (index < next_free_spot)
            next_free_spot = index;
        return NULL;
    }
    else if (action == MM_DESTROY)
        free(free_spots);
}




/********** Shared memory **********/

shmem_reg_t *shmem_regions = NULL;
char regions_size = 0;

shmem_t shmem_region_manager(char act, char alias, char ind)
{
    static int reg_next_free = 0;
    static char *translate = NULL;

    /* Nueva conexion a la region */
    if (act == SHMEM_REG_NEW) {
        shmem_t result = {0};
        
        /* Alias 0 no permitido por uso interno */
        if (alias == 0) __error("shmem alias zero", 16);
        
        /* Si la region ya existia, se devuelve un nuevo puntero */
        if (translate != NULL) {
            for (char i = 0; i < regions_size; i++) {
                if (translate[i] = alias) {
                    result.index = i;
                    return result;
                }
            }
        }
        
        /* Si no, se crea la region y se devuelve una nueva */
        /* Bloqueo de sched para proteger mallocs */
        block_sched();
        /* Chequear espacio en el array */
        if (reg_next_free >= regions_size) {
            regions_size += SHMEM_ARR_LEAP;
            shmem_regions = realloc(shmem_regions, sizeof(shmem_reg_t) * regions_size + 1);
            if (shmem_regions == NULL)
                __error("shmem malloc error", 18);
            translate = realloc(translate, sizeof(char) * regions_size + 1);
            if (translate == NULL)
                __error("shmem malloc error", 18);
            for (int i = regions_size - SHMEM_ARR_LEAP; i <= regions_size; i++) {
                (shmem_regions[i]).region = NULL;
                (shmem_regions[i]).writen = 0;
                translate[i] = 0;
            }
        }
        /* Armar la nueva region */
        void *mem_pos = malloc(SHMEM_REGION_SIZE);
        if (mem_pos == NULL)
            __error("shmem malloc error", 18);
        (shmem_regions[reg_next_free]).region = mem_pos;
        result.index = reg_next_free;
        translate[reg_next_free] = alias;
        for (int i = 0; i <= regions_size; i++) {
            if ((shmem_regions[i]).region == NULL)
                reg_next_free = i;
        }
        unblock_sched();
        return result;
    }

    /* Remover una region */
    else if (act == SHMEM_REG_DEL) {
        if (ind >= regions_size) 
            __error("shmem error: wrong index",24);
        if ((shmem_regions[ind]).region == NULL)
            __error("shmem error: not initialized",28);
        free((shmem_regions[ind]).region);
        (shmem_regions[ind]).region = NULL;
        translate[ind] = 0;
        if (ind < reg_next_free)
            reg_next_free = ind;
        return (shmem_t) {0};
    }

    /* Destruye todas las regiones y libera toda memoria reservada */
    else if (act == SHMEM_DESTROY_ALL) {
        for (int i = 0; i < regions_size; i++) {
            if ((shmem_regions[i]).region != NULL)
                free((shmem_regions[i]).region);
        }
        free(shmem_regions);
        free(translate);
        return (shmem_t) {0};
    }
}



int shmem_check(shmem_t *sm)
{
    if (regions_size <= sm->index)
        return SHMEM_CLOSED;
    else if ((shmem_regions[sm->index]).region == NULL)
        return SHMEM_CLOSED;
    else
        return SHMEM_OK;
}


int shmem_write(shmem_t *sm, char *buf, unsigned char n)
{
    /* La funcion devuelve closed si no se puede escribir
       o el numero de caracteres escritos si se pudo */
    int check = shmem_check(sm);
    if (check != SHMEM_OK)
        return check;
    if ((SHMEM_REGION_SIZE - sm->pointer) < n)
        n = SHMEM_REGION_SIZE - sm->pointer;
    char *region = (shmem_regions[sm->index]).region;
    for (char i = 0; i < n; i++)
        region[i+sm->pointer] = buf[i];
    sm->pointer += n+1;
    unsigned char writen = (shmem_regions[sm->index]).writen;
    if (sm->pointer > writen)
        (shmem_regions[sm->index]).writen += sm->pointer - writen;
    return n;
}


int shmem_read(shmem_t *sm, void *buf, unsigned char n)
{
    /* La funcion devuelve closed si no se puede leer
       o el numero de caracteres leidos si se pudo */
    int check = shmem_check(sm);
    if (check != SHMEM_OK)
        return check;
    if ((SHMEM_REGION_SIZE - sm->pointer) < n)
        n = SHMEM_REGION_SIZE - sm->pointer;
    if ((sm->pointer - (shmem_regions[sm->index]).writen) < 1)
        return 0;
    else if ((sm->pointer - (shmem_regions[sm->index]).writen) < n)
        n = sm->pointer - (shmem_regions[sm->index]).writen;
    char *region = (shmem_regions[sm->index]).region;
    for (char i = 0; i < n; i++)
        ((char *)buf)[i] = region[i+sm->pointer];
    sm->pointer += n+1;
    return n;
}


int shmem_seek(shmem_t *sm, unsigned char n, char off)
{
	/* Coloca el puntero en el lugar indicado, solo si
	   ya se escribio en ese lugar, o si es el proximo byte
	   a escribir (la escritura es secuencial). Si se indica
	   SEEK_ABS el puntero se coloca en la posicion n; si
	   no, se coloca relativamente a la posicion actual,
	   hacia adelante (SEEK_OFFSET) o atras (SEEK_REV_OFFSET) */
    int check = shmem_check(sm);
    if (check != SHMEM_OK)
        return check;
    unsigned char writen = (shmem_regions[sm->index]).writen;
    if (off == SEEK_ABS) {
        if (n > writen + 1)
            return SHMEM_INVALID;
        else {
            sm->pointer = n;
            return SHMEM_OK;
        }
    }
    else if (off == SEEK_OFFSET) {
        if (n + sm->pointer > writen + 1)
            return SHMEM_INVALID;
        else {
            sm->pointer += n;
            return SHMEM_OK;
        }
    }
    else if (off == SEEK_REV_OFFSET) {
        if (sm->pointer - n > writen + 1)
            return SHMEM_INVALID;
        else {
            sm->pointer -= n;
            return SHMEM_OK;
        }
    }
    else
        __error("shmem_seek invalid flag",23);
}


int shmem_poll(shmem_t *sm)
{
	/* Devuelve SHMEM_OK si hay algo escrito a partir de la
	   posicion actual del puntero */
    int check = shmem_check(sm);
    if (check != SHMEM_OK)
        return check;
    if ((shmem_regions[sm->index]).writen >= sm->pointer)
        return SHMEM_OK;
    else
        return SHMEM_INVALID;
}


int shmem_block_read(shmem_t *sm, void *buf, unsigned char hm)
{
    while (1) {
        // Pedir un lock
        int p = shmem_poll(sm);
        // Soltar lock
        if (p == SHMEM_INVALID)
            task_yield();
        else if (p == SHMEM_OK)
            break;
        else
            return p;
    }
    return shmem_read(sm,buf,hm);
}



