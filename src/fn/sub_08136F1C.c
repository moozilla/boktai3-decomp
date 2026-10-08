#include "global.h"
void sub_08249240(u8 *, u16 *, u32);
void sub_08136F1C(u8 *p, u16 *q)
{
    if (q != 0) {
        u32 i = q[0];
        u32 *t = (u32 *)*(u32 *)(p + 0x250);
        sub_08249240(p, q, t[i]);
    }
}
