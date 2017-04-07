#ifndef __GUINDOWS2_H
#define __GUINDOWS2_H

#include <setjmp.h>


typedef struct _task {
    jmp_buf buf;
    int level;
    int msec;
} task;


#define TASK_NEW (SIGRTMIN)
#define TASK_YIELD (SIGRTMIN+1)

#define MS 1000
#define TICK 5
#define QUANTUM 4

#define TIME_L0 (QUANTUM*TICK*MS)
#define TIME_L1 (QUANTUM*TICK*MS*2)
#define TIME_L2 (QUANTUM*TICK*MS*4)
#define TIME_L3 (QUANTUM*TICK*MS*8)

#define TPILA "4096"
#define CREATE_STACK(t, f) do{if(setjmp(t)) f(); asm("subq $"TPILA", %rsp");}while(0)
#define DESTROY_STACK(t) asm("addq $"TPILA", %rsp")

#define TRANSFER(o,d) (setjmp(o)==0?(longjmp(d,1),0):0)
#define ACTIVATE(d) TRANSFER(tsched, d)
#define YIELD(o) TRANSFER(o, tsched)

#endif//__GUINDOWS2_H
