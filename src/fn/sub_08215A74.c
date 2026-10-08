#include "global.h"

void sub_08215A74(s32 n, u32 v)
{
    vu16 *r;
    u16 t;
    switch (n) {
    case 0: r = (vu16 *)0x04000008; break;
    case 1: r = (vu16 *)0x0400000A; break;
    case 2: r = (vu16 *)0x0400000C; break;
    case 3: r = (vu16 *)0x0400000E; break;
    default: return;
    }
    t = (0xFFFC & *r) | (v );
    *r = t;
}
