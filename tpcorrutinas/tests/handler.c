
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

#define HANDY_SIG SIGRTMIN


void handythehandler(int signum, siginfo_t *data, void *extra)
{
    puts("Hello, I'm Handy");
    jmp_buf *dabuf = (data -> si_value).sival_ptr;
    printf("the buf is in %p\n", dabuf);
    longjmp(*dabuf, 1);
    return;
}


int main(void)
{
    sigset_t block_these;
    sigemptyset(&block_these);
    struct sigaction sa = {.sa_sigaction = handythehandler,
                            .sa_mask = block_these,
                            .sa_flags = SA_SIGINFO
                          };
    sigaction(HANDY_SIG, &sa, NULL);
    jmp_buf jb;
    struct rlimit rl;
    getrlimit(RLIMIT_SIGPENDING, &rl);
    printf("Limit is %ld\n",rl.rlim_cur);
    printf("the buf is in %p\n", &jb);
    union sigval sv = {.sival_ptr = &jb};
    //while (1) {
    if (setjmp(jb) == 0) {
        puts("I'm gonna call Handy");
        if (sigqueue(getpid(), HANDY_SIG, sv) != 0) {
            if (errno == EAGAIN) puts("EAGAIN");
            else if (errno == EINVAL) puts("EINVAL");
            else if (errno == EPERM) puts("EPERM");
            else if (errno == ESRCH) puts("ESRCH");
        }
    }
    else {
        puts("Handy says hi");
        sleep(2);
    }
    //}
    return 0;
}

