
all: example

example: scheduler.o pqueue.o mm.o
	gcc sched_example.c scheduler.o pqueue.o mm.o -lrt -o sched_example

scheduler.o: ../scheduler.c
	gcc -c ../scheduler.c

pqueue.o: ../pqueue.c
	gcc -c ../pqueue.c

mm.o: ../mm.c
	gcc -c ../mm.c

clean:
	rm sched_example scheduler.o pqueue.o mm.o

