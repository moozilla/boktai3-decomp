#include "global.h"

void Script_Call(u32, u32);

void sub_08078264(u8 *p)
{
    u32 v = *(u16 *)(p + 0x1f0);
    if (v)
        Script_Call(v, 0);
}
