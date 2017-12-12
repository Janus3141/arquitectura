
#ifndef __QUEUE_H
#define __QUEUE_H


#include <malloc.h>


typedef struct _q_elem {
    // Estructura para cada elemento de la cola
    void *data;
    struct _q_elem *next;
    struct _q_elem *prev;
    char lvl;
} q_elem;


typedef struct {
    // Estructura para contener el inicio y final de la cola
    q_elem *front;
    q_elem **backs;
    int *size;
    char maxlevel;
} pqueue;


pqueue queue_create(char);
/* Devuelve la cola vacia */

void queue_destroy(pqueue *);
/* Destruye la cola y todo su contenido */

void queue_insert(pqueue *, q_elem *);
/* Reinserta un q_elem previamente quitado de la cola */

void queue_new_node(pqueue *, void *);
/* Inserta un nuevo nodo en la cola 0 */

q_elem *queue_pop(pqueue *);
/* Devuelve el primer dato guardado en la cola.
   Si la cola esta vacia, se devuelve NULL */ 


#endif // __QUEUE_H

