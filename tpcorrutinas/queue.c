#include <malloc.h>
#include "queue.h"


queue queue_init(void)
{
    queue q = malloc(sizeof(struct _q));
    if (q == NULL) error("No space to allocate.");
    q->front = NULL;
    q->back = NULL;
    q->size = 0;
    return q;
}


void queue_destroy(queue q)
{
    // No se provee proteccion contra un fallo de free()
    if (q->front != NULL) {
        while (q->front.next != NULL) {
            q->front = q->front.next;
            free(q->front.prev);
        }
    }
    free(q);
}


int queue_size(queue q)
{
    return q->size;
}


void queue_insert(queue q, void *elem)
{
    // Se termina el programa en caso de un fallo en malloc
    q_elem *new = malloc(sizeof(q_elem));
    if (new == NULL) error("No space to allocate.");
    new->data = elem;
    new->next = NULL;
    new->prev = NULL;
    if (queue_size(q) == 0) {
        q->front = new;
        q->back = new;
    }
    else {
        q->back.next = new;
        new->prev = q->back;
        q->back = new;
    }
    q->size++;
    return;
}


void *queue_pop(queue q)
{
    
    if (queue_size(q) == 0)
        return NULL;
    if (queue_size(q) == 1)
        q->back = NULL;
    q->size--;
    void *r = q->front.data;
    q->front = q->front.next;
    free(q->front.prev);
    q->front.prev = NULL;
    return r;
}
