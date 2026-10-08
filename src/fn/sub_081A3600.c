#include "global.h"
struct P { u8 f[0x38]; u32 a, b; u16 x, y, z; };
s32 sub_081A3554(struct P *);
s32 sub_081A3600(struct P *p, u16 x, u16 y, u16 z)
{
    p->b = 0;
    p->a = 0;
    p->x = x;
    p->y = y;
    p->z = z;
    if (sub_081A3554(p) < 0) return -1;
    return 0;
}
