#include "global.h"
struct Q { u8 f0; u8 f1; u8 f2; u8 f3; u8 buf[16]; };
s32 sub_08224E0C(struct Q *q, void *src)
{
    if (q->f0 == 0 && q->f1 == q->f2)
        return 1;
    CpuSet(src, (u8 *)q + ((q->f2 << 4) + 4), 0x04000004);
    q->f3 = q->f3 + 1;
    q->f2 = (q->f2 + 1) & 0x1f;
    q->f0 = 0;
}
