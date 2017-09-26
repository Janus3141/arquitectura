
#ifndef __QUEUE_H
#define __QUEUE_H


#include <malloc.h>


typedef struct _q_elem {
    // Estructura para cada elemento de la cola
    void *data;
    struct _q_elem *next;
    struct _q_elem *prev;
} q_elem;


typedef struct _q {
    // Estructura para contener el inicio y final de la cola
    q_elem *front;
    q_elem *back;
    int size;
} *queue;


queue queue_init(void);
// Devuelve la cola vacia

void queue_destroy(queue);
// Destruye la cola y todo su contenido

int queue_size(queue);

void queue_insert(queue, void *);
// Inserta un elemento en la cola, se guarda como su 'back'

void *queue_pop(queue);
/*
* Devuelve el dato guardado en el front de la cola.
* Si la cola esta vacia, se devuelve NULL.
* Notar que no se diferencia entre esto y haber guardado NULL
* en la cola (lo cual pudo haber sido un error).
*/ 

#endif // __QUEUE_H
