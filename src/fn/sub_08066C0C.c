#include "global.h"

struct P { u8 f[0xa1c]; u32 v; };
extern struct P *gUnk_02000124;
void sub_08066BB8(struct P *);

s32 sub_08066C0C(struct P *p)
{
    p->v = 0;
    sub_08066BB8(p);
    gUnk_02000124 = p;
    return 0;
}
