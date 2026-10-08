#include "global.h"

struct P { u8 f00[0x4C4]; u32 w; };

void sub_0815FA78(struct P *);
void sub_0824923C(struct P *, u32);

s32 sub_0815FF98(struct P *p)
{
    sub_0815FA78(p);
    sub_0824923C(p, p->w);
    return 0;
}
