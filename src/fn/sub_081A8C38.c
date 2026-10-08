#include "global.h"
extern const u32 gUnk_08612004[];
s32 sub_081A7EA0(u8 *);
void sub_0824923C(u8 *, u32);
void sub_081A8C38(u8 *p)
{
    if (sub_081A7EA0(p) == 0)
        sub_0824923C(p, gUnk_08612004[*(u8 *)(p + 0xad)]);
}
