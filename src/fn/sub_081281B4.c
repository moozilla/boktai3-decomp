#include "global.h"
void Script_Call(u32, u32);
void sub_081281B4(u8 *p)
{
    if (*(u32 *)(p + 0x158))
        Script_Call(*(u32 *)(p + 0x158), 0);
}
