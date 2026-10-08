#include "global.h"

extern void *gUnk_020001AC;

u32 sub_0821A520(u32, u32);

s32 sub_08103A54(void *p)
{
    *(u32 *)((u8 *)p + 0x18) = sub_0821A520(0x922E, 0x13F9);
    gUnk_020001AC = p;
    return *(u32 *)((u8 *)p + 0x136C) = 0;
}
