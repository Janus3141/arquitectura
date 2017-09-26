/* Memory management */

#define TPILA 4096


void take_stack(void)
{
    asm("subq $"TPILA", %rsp");
}

