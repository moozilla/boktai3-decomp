#include "global.h"

struct S { u8 f[0x68]; s32 a; };
struct T { u8 f[8]; s16 a; };
void sub_08020D68(u32, u32);
void sub_08020AC0(struct S *p, u32 x, struct T *t)
{
    s32 v = t->a;
    p->a = v;
    if (v == 0)
        sub_08020D68(x, 1);
}
