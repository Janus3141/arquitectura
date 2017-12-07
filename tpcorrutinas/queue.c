
#include <malloc.h>
#include "queue.h"
#include <stdlib.h>
#include <unistd.h>


void __q_error(char *m, size_t size)
{
    write(STDERR_FILENO, m, size);
    _Exit(EXIT_FAILURE);
}


multi_queue queue_create(char n)
{
    q_elem **backs = calloc(n, sizeof(q_elem *));
    if (backs == NULL) __q_error("No space to allocate", 20);
    char *sizes = calloc(n, sizeof(char));
    if (sizes == NULL) __q_error("No space to allocate", 20);
    multi_queue mq = {.front = NULL,
                      .backs = backs,
                      .size = sizes,
                      .maxlevel = n-1};
    return mq;
}


void queue_destroy(multi_queue *q)
{
    // No se provee proteccion contra un fallo de free
    if (q -> front != NULL) {
        while ((q -> front) -> next != NULL) {
            q -> front = (q -> front) -> next;
            free((q -> front) -> prev);
        }
        free(q->front);
    }
    free(q -> backs);
    free(q -> size);
    return;
}


void queue_insert(multi_queue *q, q_elem *elem)
{
    // El caller debe ocuparse del nivel en que se inserta elem
    if ((q->size)[elem->lvl] == 0) {
        (q -> backs)[elem -> lvl] = elem;
        if (q -> front == NULL)
            q -> front = elem;
        else if ((q -> front) -> lvl > elem -> lvl) {
            /* En este caso, q->front->lvl != 0 y elem->lvl != maxlvl */
            elem -> next = q -> front;
            q -> front = elem;
        }
        else {
            /* Busco vecinos hacia el frente de la cola. Seguro hay
               por condicion anterior false */
            for (char i = elem->lvl - 1; i >= 0; i--) {
                if ((q->size)[i] != 0) {
                    elem -> next = (q->backs)[i] -> next;
                    elem -> prev = (q->backs)[i];
                    (q->backs)[i] -> next = elem;
                    break;
                }
            }
        }
    }
    else {
        elem -> next = (q->backs)[elem->lvl] -> next;
        elem -> prev = (q->backs)[elem->lvl];
        (q->backs)[elem->lvl] -> next = elem;
        (q->backs)[elem->lvl] = elem;
    }
    (q->size)[elem->lvl]++;
    return;
}


void queue_new_node(multi_queue *q, void *elem)
{
    q_elem *new = malloc(sizeof(q_elem));
    new -> data = elem;
    new -> lvl = 0;
    if ((q->size)[0] > 0) {
        new -> next = (q->backs)[0] -> next;
        new -> prev = (q->backs)[0];
        (q->backs)[0] -> next = new;
        (q->backs)[0] = new;
    }
    else {
        new -> next = q -> front;
        new -> prev = NULL;
        (q->backs)[0] = new;
        q -> front = new;
    }
    (q->size)[0]++;
    return;
}


q_elem *queue_pop(multi_queue *q)
{
    q_elem *poped = q->front;
    if (poped != NULL) {
        (q->size)[poped->lvl]--;
        q -> front = poped -> next;
        poped -> prev = NULL;
        poped -> next = NULL;
    }
    return poped;
}


