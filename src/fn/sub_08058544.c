#include "global.h"

struct S { u8 f[0xc10]; u8 m; };

s32 sub_08058544(struct S *p)
{
    s32 r;
    switch (p->m) {
    case 0: r = 8; break;
    case 1:
    case 2: r = 6; break;
    case 3: r = 4; break;
    case 4: r = 2; break;
    case 7:
    case 8: r = 10; break;
    case 9:
    case 10: r = 12; break;
    default: r = 0; break;
    }
    return r;
}
