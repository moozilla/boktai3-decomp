#include "global.h"
void Script_Call(u32, u32);
void sub_0806CFE4(u8 *p)
{
    u32 v = *(u32 *)(p + 0x170);
    if (v != 0)
        Script_Call(v, 0);
}
