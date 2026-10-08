#include "global.h"

void sub_08217084(u32 v)
{
    *(volatile u16 *)0x04000054 = v;
}
