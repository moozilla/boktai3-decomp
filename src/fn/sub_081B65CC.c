#include "global.h"
s32 sub_081B65CC(u8 *p)
{
    u32 m = 0x10;
    if (*(u32 *)(p + 0x1f8) & m) return 0;
    if (*(u32 *)(p + 0x414) & m) return 1;
    return -1;
}
