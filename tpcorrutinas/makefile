
.RECIPEPREFIX = >

# all: example

ex1: scheduler.o pqueue.o mm.o locks.o gerror.o
> gcc sched_example.c source/scheduler.o source/pqueue.o source/mm.o source/locks.o source/gerror.c -lrt -g -o sched_example

ex2: scheduler.o pqueue.o mm.o locks.o gerror.o
> gcc sched_example2.c source/scheduler.o source/pqueue.o source/mm.o source/locks.o source/gerror.c -lrt -g -o sched_example

ex3: scheduler.o pqueue.o mm.o locks.o gerror.o
> gcc sched_example3.c source/scheduler.o source/pqueue.o source/mm.o source/locks.o source/gerror.c -lrt -g -o sched_example

scheduler.o: source/scheduler.c
> gcc -g -c source/scheduler.c -o source/scheduler.o

pqueue.o: source/pqueue.c
> gcc -g -c source/pqueue.c -o source/pqueue.o

mm.o: source/mm.c
> gcc -g -c source/mm.c -o source/mm.o

locks.o: source/locks.c
> gcc -g -c source/locks.c -o source/locks.o

gerror.o: source/gerror.c
> gcc -g -c source/gerror.c -o source/gerror.o

clean:
> rm sched_example source/scheduler.o source/pqueue.o source/mm.o source/locks.o source/gerror.o

