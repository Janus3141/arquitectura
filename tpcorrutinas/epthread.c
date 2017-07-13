#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


void *routine(void *algo)
{
    for (int i = 0; i < 5; i++)
        puts((char *)algo);
    return NULL;
}


int main()
{
    pthread_t p1,p2,p3,p4,p5;
    
    /*
       int pthread_create(pthread_t *thread,
                          pthread_attr_t *attr,
                          void *(*routine) (void *),
                          void *arg)
    */

    pthread_create(&p1,NULL,routine,"Mao");
    pthread_create(&p2,NULL,routine,"Macri");
    pthread_create(&p3,NULL,routine,"Trump");
    pthread_create(&p4,NULL,routine,"Obama");
    pthread_create(&p5,NULL,routine,"Merkel");

    // int pthread_join(pthread_t thread, void **retval)

    pthread_join(p1,NULL);
    pthread_join(p2,NULL);
    pthread_join(p3,NULL);
    pthread_join(p4,NULL);
    pthread_join(p5,NULL);

    return 0;
}
