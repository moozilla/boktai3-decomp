#include "global.h"
void Memory_Clear(void *, u32);
void Trap_Init(void *p)
{
    Memory_Clear(p, 0x2C);
}
