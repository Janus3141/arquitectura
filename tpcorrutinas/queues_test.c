
#include "queue.h"
#include <stdio.h>


int main(void) {
    char db[] = "deadbeef";
    multi_queue q = queue_create(3);
    for (int i = 0; i <= 9; i++)
        queue_new_node(&q, &(db[i]));
    for (int i = 0; i <=9; i++)
        printf("%d°> %c\n", i, *((char *) (queue_pop(&q)->data)));
    return 0;
}
