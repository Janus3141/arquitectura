
/********** Priority queues header **********/

#ifndef __QUEUE_H
#define __QUEUE_H


#include <malloc.h>


/* Estructura para elementos de la cola */
typedef struct _q_elem {
    void *data;
    struct _q_elem *next;
    struct _q_elem *prev;
    char lvl;
} q_elem;


/* Estructura para contener el inicio y final de la cola */
typedef struct {
    q_elem *front;
    q_elem **backs;
    int *size;
    char maxlevel;
} pqueue;


/* Devuelve la cola vacia */
pqueue queue_create(char);

/* Destruye la cola y todo su contenido */
void queue_destroy(pqueue *);

/* Reinserta un q_elem previamente quitado de la cola */
void queue_insert(pqueue *, q_elem *);

/* Inserta un nuevo nodo con prioridad 0 */
void queue_new_node(pqueue *, void *);

/* Devuelve el primer dato guardado en la cola.
   Si la cola esta vacia, se devuelve NULL */ 
q_elem *queue_pop(pqueue *);

/* Eleva la prioridad de todos los elementos guardados a 0 */
void queue_lift(pqueue *);


#endif // __QUEUE_H

