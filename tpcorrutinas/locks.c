
#include "queue.h"


typedef struct {
    queue qu;
    short key : 1;
} lock;



lock *lock_init(void)
{
    lock *l = malloc(sizeof(lock));
    queue new = queue_init();
    l->qu = new;
    l->key = 1;
    return l;
}


void lock_destroy(lock *l)
{
    queue_destroy(l->qu);
    free(l);
    l = NULL;
    return;
}


void lock_manager(task *t, int n)
{
    if (n == 0) {
        
    else if (n == 1)
