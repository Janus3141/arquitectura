// SEÑALES

#include <unistd.h>
#include <signal.h>

// sigset_t: Conjunto de señales

int sigemptyset(sigset_t *set); // Vacía 'set'

int sigfillset(sigset_t *set); // Llena 'set'

int sigaddset(sigset_t *set, int sig); // Añade la señal 'sig' a 'set'

int sigdelset(sigset_t *set, int sig); // Quita la señal 'sig' de 'set'

int sigprocmask(int op, sigset_t *set, sigset_t *oldset);
/*
'op' puede ser:
    SIG_SETMASK: Bloque las señales de 'set' mandadas al proceso
    SIG_BLOCK: Bloque las señales de 'set' mandadas al proceso (se suman a las que ya estaban bloqueadas)
    SIG_UNBLOCK: Desbloquea las señales en 'set' (si no estaban bloqueadas no pasa nada)
En 'oldset' se devuelven las señales bloqueadas antes de llamar a la función
*/

// Las funciones anteriores devuelven 0 si todo fue correcto, -1 por error

int sigismember(sigset_t *set, int sig); // Devuelve 1 si 'sig' esta en 'set', 0 si no, y -1 si hubo error

union sigval {
    int sival_int;
    void *sival_ptr;
}

typedef struct {
    int si_signo; // Señal que se recibe
    int si_code; // De donde viene la señal (timer, kill, etc.)
    union sigval si_value; // Extra data
} siginfo_t;

struct sigaction {
    void (*sa_handler) (int); // Para señales POSIX 1
    void (*sa_sigaction) (int, siginfo_t *, void *); // Handler para señales POSIX 4, usamos estas
    sigset_t sa_mask; // Señales a bloquear durante ejecución del handler
    int sa_flags; // Hay varias, se pueden unir con OR. Usamos SA_SIGINFO para señales POSIX 4
    void (*sa_restorer) (void); // No tocar
};

int sigaction(int this_signal, struct sigaction *sa, struct sigaction *old_response);
/* A la llegada de 'this_signal' se llama al handler
    especificado en 'sa', con todas las opciones seteadas
    en esa estructura. En 'old_response' se devuelve la
    estructura usada anteriormente para 'this_signal'.
*/

int kill(pid_t victim_id, int this_signal); // Manda 'this_signal' al proceso con ID 'victim_id'

// POSIX 4

int sigqueue(pid_t victim_id, int this_signal, union sigval extra_info);
// Manda 'this_signal' al proceso 'victim_id' con la información extra 'extra_info'

// Otras funciones: sigsuspend, sigwaitinfo, sigtimedwait.

/* 
Las señales POSIX 4 (SIGRTMIN ... SIGRTMAX) se encolan en el proceso,
como máximo puede haber SIGQUEUE_MAX señales en la cola de señales.
Además se entregan en orden, la de menor número primero, o sea
SIGRTMIN tiene prioridad de entrega sobre SIGRTMAX.
