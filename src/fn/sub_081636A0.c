#include "global.h"
struct P { u8 f0[0x340]; u8 x; };
extern const u32 gUnk_08610E64[];
void sub_0824923C(struct P *, u32);
s32 sub_081636A0(struct P *p)
{
    sub_0824923C(p, gUnk_08610E64[p->x]);
    return 0;
}
