
all: example

example: scheduler.o pqueue.o mm.o
	gcc sched_example2.c scheduler.o pqueue.o mm.o -lrt -g -o sched_example

scheduler.o: ../scheduler.c
	gcc -g -c ../scheduler.c

pqueue.o: ../pqueue.c
	gcc -g -c ../pqueue.c

mm.o: ../mm.c
	gcc -g -c ../mm.c

clean:
	rm sched_example scheduler.o pqueue.o mm.o

