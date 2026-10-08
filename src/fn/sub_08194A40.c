#include "global.h"
struct H { u8 f[6]; u16 v; };
struct W { u32 a; u32 v; };
static inline void orr(struct W *a, u32 m) { a->v |= m; }
s32 sub_08190C98(void *, s32, s32);
void sub_08194A40(u8 *p)
{
    if (p[0x724B] != 0) sub_08190C98(p, 0, 0x38);
    else sub_08190C98(p, 0, 0x13);
    ((struct H *)p)[3291].v |= 4;
    ((struct H *)p)[3018].v |= 4;
    orr(&((struct W *)p)[3564], 1);
}
