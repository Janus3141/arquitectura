
.RECIPEPREFIX = >

CC = gcc

PROGRAM = sched_example

IDIR = include
ODIR = obj
SDIR = src
EDIR = examples

CFLAGS = -I$(IDIR) -g

LIBS = -lrt

_DEPS = scheduler.h pqueue.h shmem.h gerror.h locks2.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = scheduler.o pqueue.o shmem.o gerror.o locks2.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

ASM = $(SDIR)/jmp.s $(SDIR)/locks.s

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
> $(CC) -c $< -o $@ $(CFLAGS)


ex1 ex2 ex3 ex4 ex5 ex6 ex7: $(OBJ) $(ASM)
> $(CC) $(subst ex,$(EDIR)/sched_example,$@.c) -o $(PROGRAM) $^ $(CFLAGS) $(LIBS) 


clean:
> rm -f $(ODIR)/*.o $(PROGRAM)

