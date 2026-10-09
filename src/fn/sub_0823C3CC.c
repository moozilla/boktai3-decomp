#include "global.h"
void sub_082383D4(u8 *, s32, s32);
s32 sub_0823C368(u8 *);
void sub_08238238(u8 *);
s32 sub_081419D4(u32, s32);
void sub_08238280(u8 *, s32, s32);
void sub_0823C3CC(u8 *p)
{
    s32 value;
    if (p[0x457]) {
        sub_082383D4(p, 0, 0);
        p[0x3a4] = sub_0823C368(p);
        sub_08238238(p);
    }
    *(u32 *)(p + 0x20) &= ~1;
    value = sub_081419D4(p[0x418], 0);
    sub_08238280(p, value, 0x40);
}
