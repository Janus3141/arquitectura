
/********** Shared memory **********/

#include "shmem.h"
#include "scheduler.h"
#include "gerror.h"
#include <stdlib.h>
#include <malloc.h>


/*** Globales ***/
char regions_size = 0;
shmem_reg_t *shmem_reg_arr = NULL;

/*** Acceso ***/
#define region(i) ((shmem_reg_arr[i]).region)
#define writen(i) ((shmem_reg_arr[i]).writen)


/*** Definicion de funciones ***/

shmem_t shmem_region_manager(char act, int alias, shmem_t *sh)
{
    static int reg_next_free = 0;

    /* Nueva conexion a la region */
    if (act == SHMEM_REG_NEW) {
        shmem_t result = {0};
        
        /* Alias 0 no permitido por uso interno */
        if (alias == 0) __error("shmem alias zero", 16);
        
        /* Bloqueo de sched para prevenir lecturas incorrectas
            y proteger mallocs */
        block_sched();
        /* Si la region ya existia, se devuelve un nuevo puntero */
        if (shmem_reg_arr != NULL) {
            for (char i = 0; i < regions_size; i++) {
                if ((shmem_reg_arr[i]).alias == alias) {
                    result.index = i;
                    return result;
                }
            }
        }
        
        /* Si no, se crea la region y se devuelve una nueva */
        /* Chequear espacio en el array, si no hay
            se reservan SHMEM_ARR_LEAP mas */
        if (reg_next_free >= regions_size) {
            int i = regions_size;
            regions_size += SHMEM_ARR_LEAP;
            shmem_reg_arr = realloc(shmem_reg_arr, sizeof(shmem_reg_t) * (regions_size + 1));
            if (shmem_reg_arr == NULL)
                __error("shmem malloc error", 18);
            for (; i <= regions_size; i++) 
                shmem_reg_arr[i] = (shmem_reg_t) {0};
        }
        void *new_reg = malloc(SHMEM_REGION_SIZE);
        if (new_reg == NULL)
            __error("shmem malloc error", 18);
        (shmem_reg_arr[reg_next_free]).region = new_reg;
        (shmem_reg_arr[reg_next_free]).alias = alias;
        (shmem_reg_arr[reg_next_free]).connections++;
        result.index = reg_next_free;
        for (int i = 0; i <= regions_size; i++) {
            if ((shmem_reg_arr[i]).alias == 0)
                reg_next_free = i;
        }
        unblock_sched();
        return result;
    }

    /* Desconexion de una region. Si no hay mas conexiones se marca
        la region como libre. */
    else if (act == SHMEM_REG_DISC) {
        if ((shmem_reg_arr[sh -> index]).alias == 0)
            __error("shmem wrong index", 17);
        if (--(shmem_reg_arr[sh -> index]).connections <= 0) {
            (shmem_reg_arr[sh -> index]).alias = 0;
            (shmem_reg_arr[sh -> index]).writen = 0;
            (shmem_reg_arr[sh -> index]).connections = 0;
            if (sh -> index < reg_next_free)
                reg_next_free = sh -> index;
        }
        return (shmem_t) {0};
    }

    /* Libera toda memoria reservada */
    else if (act == SHMEM_DESTROY_ALL) {
        for (int i = 0; i < regions_size; i++) {
            if ((shmem_reg_arr[i]).region != NULL)
                free((shmem_reg_arr[i]).region);
        }
        free(shmem_reg_arr);
        return (shmem_t) {0};
    }
}




int shmem_check(shmem_t *sm)
{
    /* Chequea que se pueda acceder al area dada. Si la tarea cerro
        la conexion anteriormente pero la region sigue activa,
        esta funcion aun devolvera SHMEM_OK */
    if (regions_size <= sm->index)
        return SHMEM_CLOSED;
    else if ((shmem_reg_arr[sm->index]).alias == 0)
        return SHMEM_CLOSED;
    else if ((shmem_reg_arr[sm->index]).region == NULL)
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
    /* Chequear espacio restante en la region. Se toma el minimo entre
        este y n */
    if ((SHMEM_REGION_SIZE - sm->pointer) < n)
        n = SHMEM_REGION_SIZE - sm->pointer;
    /* Escribir en la region */
    for (char i = 0; i < n; i++)
        (region(sm->index))[sm->pointer++] = buf[i];
    /* Modificar estado de escritura. Tener en cuenta que se pudo
        haber llamado a seek, por lo cual writen puede no cambiar
        en n unidades */
    if (sm->pointer > writen(sm->index))
        writen(sm->index) = sm->pointer - 1;
    return n;
}


int shmem_read(shmem_t *sm, char *buf, unsigned char n)
{
    /* La funcion devuelve closed si no se puede leer
       o el numero de caracteres leidos si se pudo */
    int check = shmem_check(sm);
    if (check != SHMEM_OK)
        return check;
    /* Chequear que exista algo para leer */
    if (writen(sm->index) - sm->pointer < 0)
        return 0;
    /* Y setear n en el minimo entre lo que se puede leer y lo indicado */
    else if (writen(sm->index) - sm->pointer + 1 < n)
        n = writen(sm->index) - sm->pointer + 1;
    for (char i = 0; i < n; i++)
        buf[i] = (region(sm->index))[sm->pointer++];
    return n;
}


unsigned char shmem_size(shmem_t *sm)
{
    return writen(sm->index);
}


int shmem_seek(shmem_t *sm, int n, char off)
{
	/* Coloca el puntero en el lugar indicado, solo si ya se escribio en
        ese lugar, o si es el proximo byte a escribir (la escritura es
        secuencial). Si se indica SEEK_ABS el puntero se coloca en la
        posicion n; si se indica SEEK_OFFSET, se coloca relativamente a
        la posicion actual */
    int check = shmem_check(sm);
    if (check != SHMEM_OK)
        return check;
    if (off == SEEK_ABS) {
        if (n > writen(sm->index) + 1 || n < 0)
            return SHMEM_INVALID;
        else {
            sm->pointer = n;
            return SHMEM_OK;
        }
    }
    else if (off == SEEK_OFFSET) {
        if (n + sm->pointer > writen(sm->index) + 1 ||
            n + sm->pointer < 0)
            return SHMEM_INVALID;
        else {
            sm->pointer += n;
            return SHMEM_OK;
        }
    }
    else
        return SHMEM_INVALID;
}


int shmem_poll(shmem_t *sm)
{
	/* Devuelve SHMEM_OK si hay algo escrito a partir de la
	   posicion actual del puntero */
    int check = shmem_check(sm);
    if (check != SHMEM_OK)
        return check;
    if (writen(sm->index) >= sm->pointer)
        return SHMEM_OK;
    else
        return SHMEM_INVALID;
}


int shmem_block_read(shmem_t *sm, char *buf, unsigned char hm)
{
    while (1) {
        int p = shmem_poll(sm);
        if (p == SHMEM_INVALID)
            task_yield();
        else if (p == SHMEM_OK)
            return shmem_read(sm,buf,hm);
        else
            return p;
    }
}



