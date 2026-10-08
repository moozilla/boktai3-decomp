#include "global.h"

struct P0823D370 { u8 f0[0x20]; u32 f20; u8 f24[0x428 - 0x24]; u16 f428; u16 f42A; };
static inline u32 tst(u32 *a, u32 m) { return *a & m; }

s32 sub_0823D370(struct P0823D370 *p)
{
    if (tst(&p->f20, 0x10) == 0 && p->f428 < p->f42A)
        return 1;
    return 0;
}
