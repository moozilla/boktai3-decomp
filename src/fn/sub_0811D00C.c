#include "global.h"
struct S { u8 f[0x10]; s32 x; s32 y; s32 z; u8 g[0x1c]; s32 w; };
void sub_0811D00C(struct S *s, s16 *o)
{
    o[0] = s->x >> 12;
    o[1] = (s->y + s->w) >> 12;
    o[2] = s->z >> 12;
}
