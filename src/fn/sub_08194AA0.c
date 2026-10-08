#include "global.h"
struct H { u8 f[6]; u16 v; u8 g[4]; };
struct W { u32 a; u32 v; };
static inline void orr(struct W *a, u32 m) { a->v |= m; }
s32 sub_08190C98(void *, s32, s32);
void sub_08194AA0(u8 *p)
{
    if (p[0x724B] != 0) sub_08190C98(p, 1, 0x36);
    else sub_08190C98(p, 1, 0x15);
    ((struct H *)p)[2201].v |= 4;
    ((struct H *)p)[2019].v |= 4;
    orr(&((struct W *)p)[3566], 1);
}
