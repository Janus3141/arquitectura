
#include "queue.h"
#include <stdio.h>


int main(void) {
    char db[] = "deadbeef";
    multi_queue q = queue_create(4);
    for (int i = 0; i <= 7; i++)
        queue_new_node(&q, &(db[i]));
    q_elem *d = queue_pop(&q);
    q_elem *e = queue_pop(&q);
    q_elem *a = queue_pop(&q);
    q_elem *d2 = queue_pop(&q);
    q_elem *b = queue_pop(&q);
    q_elem *e1 = queue_pop(&q);
    q_elem *e2 = queue_pop(&q);
    q_elem *f = queue_pop(&q);
    printf("ninth pop is %p\n", queue_pop(&q));
    printf("tenth pop is %p\n", queue_pop(&q));
    f->lvl = 2;
    e2->lvl = 0;
    e1->lvl = 0;
    b->lvl = 2;
    d->lvl = 1;
    e->lvl = 0;
    a->lvl = 2;
    d2->lvl = 1;
    queue_insert(&q,f);
    queue_insert(&q,e2);
    queue_insert(&q,e1);
    queue_insert(&q,b);
    queue_insert(&q,d);
    queue_insert(&q,e);
    queue_insert(&q,a);
    queue_insert(&q,d2);
    for (int i = 0; i <=7; i++)
        printf("%d°> %c\n", i, *((char *) (queue_pop(&q)->data)));
    return 0;
}
