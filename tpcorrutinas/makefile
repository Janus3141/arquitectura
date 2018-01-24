
.RECIPEPREFIX = >

# all: example


ex1: scheduler.o pqueue.o shmem.o locks.o gerror.o
> gcc sched_example.c src/scheduler.o src/pqueue.o src/shmem.o src/locks.o src/gerror.o -lrt -g -o sched_example

ex2: scheduler.o pqueue.o shmem.o locks.o gerror.o
> gcc sched_example2.c src/scheduler.o src/pqueue.o src/shmem.o src/locks.o src/gerror.o -lrt -g -o sched_example

ex3: scheduler.o pqueue.o shmem.o locks.o gerror.o
> gcc sched_example3.c src/scheduler.o src/pqueue.o src/shmem.o src/locks.o src/gerror.o -lrt -g -o sched_example

ex4:  scheduler.o pqueue.o shmem.o locks.o gerror.o
> gcc sched_example4.c src/scheduler.o src/pqueue.o src/shmem.o src/locks.o src/gerror.o -lrt -g -o sched_example

ex5:  scheduler.o pqueue.o shmem.o locks.o gerror.o
> gcc sched_example5.c src/scheduler.o src/pqueue.o src/shmem.o src/locks.o src/gerror.o -lrt -g -o sched_example

ex6:  scheduler.o pqueue.o shmem.o locks.o gerror.o
> gcc sched_example6.c src/scheduler.o src/pqueue.o src/shmem.o src/locks.o src/gerror.o -lrt -g -o sched_example


scheduler.o: src/scheduler.c
> gcc -g -c src/scheduler.c -o src/scheduler.o

pqueue.o: src/pqueue.c
> gcc -g -c src/pqueue.c -o src/pqueue.o

shmem.o: src/shmem.c
> gcc -g -c src/shmem.c -o src/shmem.o

locks.o: src/locks.c
> gcc -g -c src/locks.c -o src/locks.o

gerror.o: src/gerror.c
> gcc -g -c src/gerror.c -o src/gerror.o

clean:
> rm sched_example src/scheduler.o src/pqueue.o src/shmem.o src/locks.o src/gerror.o

