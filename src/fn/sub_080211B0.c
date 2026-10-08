#include "global.h"

struct S { u8 f[0x50]; s32 a; s32 b; };
struct Q { u8 f[8]; s16 a; s16 b; };
void sub_080211B0(struct S *p, u32 x, struct Q *q)
{
    p->a = q->a;
    p->b = q->b;
}
