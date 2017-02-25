#include <stdio.h>
#include <signal.h>
#include "guindows2.h"
#include <string.h>


struct sigaction {
	void (*sa_handler)(int);      /* address of signal handler */
	sigset_t sa_mask;                 /* additional signals to block */
	int sa_flags;                /* signal options */
	void (*sa_sigaction)(int, siginfo_t *, void*);
};


task t1, t2, tmain, tsched;

static int i;

void fsched(){
	for(;;){
		if(i++%2) ACTIVATE(t1);
		else ACTIVATE(t2);
	}
}

void hacer_proceso(char *name) {
    pid_t p;
    p = fork();
    if (p < 0)
        fprintf(stderr, "Error en creación de proceso.\n");
    else if (p == 0)
        execlp(name,NULL);
    else
        wait(0);
    return;
}

void handler(int sig){
	alarm(1);
	if(i % 2) YIELD(t2);
	else YIELD(t1);
}

int main(void) {
	stack(t1, ft1);
	stack(t2, ft2);
	stack(tsched, fsched);

	struct sigaction act;
	memset(&act,'\0', sizeof(act));
        act.sa_handler = &handler;

        act.sa_flags = SA_NODEFER;
        if (sigaction(SIGALRM, &act, NULL) < 0) {
                perror ("sigaction");
                return 1;
        }

	alarm(1);
	TRANSFER(tmain, t1);
	return 0;
}

//MULTITAREA APROPIATIVA
//un sistema op moderno suele usar un mecanismo similar (basado en temporizadores e interrupciones) para proveer multitarea. A esto se le llama multitarea apropiativa
//(Los procesos no se enteran ni tienen que hacer nada para convivir con otros procesos).
//El SO cambia de tarea de prepo (preempt) y de forma transparente.
