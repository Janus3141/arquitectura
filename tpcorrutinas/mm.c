
/* Memory management */

#include "noqueuesched.h"


void take_stack(void)
{
    asm("subq $"TPILA", %rsp");
}


void _mem_alloc(void)
{
    char rsp_sub[20];
    sprintf(rsp_sub, "subq $%d, %rsp", _ROUTINE_STACK_SIZE*10);
    asm(rsp_sub);
}


void *_mem_management(short action)
{
    /* Send 0 as action to request space,
    * else send the returned address from this function
    * to free it */
    static void *mem_containers = _mem_alloc();
    static _mem_state *states = calloc(10, sizeof(_mem_state));
    if (action == 0) 
}
