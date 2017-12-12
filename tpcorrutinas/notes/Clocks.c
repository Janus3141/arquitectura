
#include <time.h>

// POSIX 1

// time_t: Tiempo en segundos. Un entero.
// Epoch: 1 de Enero de 1970

time_t time(time_t *arg_innecesario); 
// Devuelve el tiempo en segundos desde Epoch. También lo guarda en el argumento.

unsigned int sleep(unsigned int seconds); 
/*
    "Duerme" al proceso por 'seconds' segundos. El proceso se puede despertar mientras duerme
    por el arrivo de señales, si es el caso sleep devuelve la cantidad de segundos que
    le faltaron dormir.
*/

// CLOCKS

#include <unistd.h>
#include <signal.h>

struct timespec {
    time_t tv_sec; // Segundos
    long tv_nsec; // Nanosegundos
}

struct itimerspec {
    struct timespec it_value;
    struct timespec it_interval;
}
/*
    Estructura para timers. El timer expira por primera vez en el tiempo dado en it_value.
    Después expira periódicamente en el tiempo dado en it_interval.
*/

// clockid_t: Un reloj. Usamos CLOCK_REALTIME porque es el reloj del sistema.

int clock_settime(clockid_t clock_id, struct timespec *some_time);
// Setea el reloj 'clock_id' del proceso (no del SO) en el tiempo dado en 'some_time'

int clock_gettime(clockid_t clock_id, struct timespec *current_time);
// Como time(), devuelve el tiempo actual en 'current_time'

union sigval {
    int sival_int;
    void *sival_ptr;
};

struct sigevent {
    int sigev_notify; // Metodo de notificacion. Usamos SIGEV_SIGNAL.
    int sigev_signo; // Señal a enviar.
    union sigval sigev_value; // Extra data para la señal a enviar.
};

// Declarar timer_t antes para pasarlo como 'timer_id'
int timer_create(clockid_t clock_id, struct sigevent *signal_spec, timer_t *timer_id);
/*
    Crea el timer, guarda la configuración en 'timer_id'. Cuando se setee el reloj,
    manda la señal dada en 'signal_spec' en el tiempo dado en timer_settime().
*/

int timer_settime(timer_t timer_id, int flags, struct itimerspec *new_interval, struct itimerspec *old_interval);
/*
    Pone en marcha el 'timer_id' con la configuración dada en timer_create(). El timer expira
    en el tiempo dado en new_interval, y luego periódicamente en el tiempo dado en 'old_interval'.
    'flags' se deja en 0 para comportamiento default. Dejarlo así.
    En 'old_interval' se devuelve el intervalo utilizado para el reloj antes de llamar a esta func.
*/

int timer_delete(timer_t timer_id);

// Otras funciones: timer_getoverrun, timer_gettime, nanosleep

/* 
    El sigev_value de la struct sigevent que se pasa como argumento a timer_create()
    va a llegar al handler de la señal en 'data -> si_value'
*/
