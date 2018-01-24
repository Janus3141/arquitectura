
/********** Priority queues *********/

#include <malloc.h>
#include <stdlib.h>
#include "pqueue.h"
#include "gerror.h"


pqueue *queue_create(char n)
{
    if (n <= 0) __error("queue_create invalid argument",29);
    pqueue *pq = malloc(sizeof(pqueue));
    if (pq == NULL) __error("pqueue malloc error", 19);
    q_elem **backs = calloc(n, sizeof(q_elem *));
    if (backs == NULL) __error("pqueue malloc error", 19);
    int *sizes = calloc(n, sizeof(char));
    if (sizes == NULL) __error("pqueue malloc error", 19);
    pq -> front = NULL;
    pq -> backs = backs;
    pq -> size = sizes;
    pq -> maxlevel = n-1;
    return pq;
}


void queue_destroy(pqueue *q)
{
    q_elem *helper;
    while (q -> front != NULL) {
        helper = q -> front;
        q -> front = (q -> front) -> next;
        free(helper);
    }
    free(q -> backs);
    free(q -> size);
    free(q);
    return;
}


void queue_insert(pqueue *q, q_elem *elem)
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
               por condicion anterior falsa */
            for (char i = elem->lvl - 1; i >= 0; i--) {
                if ((q->size)[i] != 0) {
                    elem -> next = (q->backs)[i] -> next;
                    (q->backs)[i] -> next = elem;
                    (q->backs)[i] = elem;
                    break;
                }
            }
        }
    }
    else {
        elem -> next = (q->backs)[elem->lvl] -> next;
        (q->backs)[elem->lvl] -> next = elem;
        (q->backs)[elem->lvl] = elem;
    }
    (q->size)[elem->lvl]++;
    return;
}


void queue_new_node(pqueue *q, void *elem)
{
    q_elem *new = malloc(sizeof(q_elem));
    if (new == NULL)
        __error("queue malloc error", 18);
    new -> data = elem;
    new -> lvl = 0;
    if ((q->size)[0] > 0) {
        new -> next = (q->backs)[0] -> next;
        (q->backs)[0] -> next = new;
        (q->backs)[0] = new;
    }
    else {
        new -> next = q -> front;
        (q->backs)[0] = new;
        q -> front = new;
    }
    (q->size)[0]++;
    return;
}


q_elem *queue_pop(pqueue *q)
{
    q_elem *poped = q->front;
    if (poped != NULL) {
        (q->size)[poped->lvl]--;
        q -> front = poped -> next;
        poped -> next = NULL;
    }
    return poped;
}


void queue_lift(pqueue *q)
{
    int total_size = 0;
    for (int i = 0; i <= q->maxlevel; i++) {
        total_size += (q->size)[i];
        (q->size)[i] = 0;
    }
    if (total_size <= 0)
        return;
    (q->size)[0] = total_size;
    q_elem *iter = q->front;
    while (iter->next != NULL) {
        iter->lvl = 0;
        iter = iter -> next;
    }
    iter -> lvl = 0;
    (q->backs)[0] = iter;
    for (int i = 1; i <= q->maxlevel; i++)
        (q->backs)[i] = NULL;
    return;
}



