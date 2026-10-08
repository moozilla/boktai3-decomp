#include "global.h"

struct P { u8 f[0xd08]; u32 v[7]; };
void sub_0803381C(u32);

void sub_0810DA38(struct P *p)
{
    u32 *q;
    s32 i;
    sub_0803381C(p->v[0]);
    i = 0;
    q = &p->v[1];
    do {
        s32 n = i + 1;
        sub_0803381C(*q++);
        i = n;
    } while (i <= 5);
}
