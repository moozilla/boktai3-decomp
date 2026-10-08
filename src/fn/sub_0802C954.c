#include "global.h"

struct S { u8 f[0x4c]; s32 a; };
void sub_0802C954(struct S *p, s32 d)
{
    p->a -= d;
    if (p->a < 0) p->a = 0;
}
