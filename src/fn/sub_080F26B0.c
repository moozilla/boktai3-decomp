#include "global.h"

u32 sub_0824923C(void *, u32);
void sub_080F2700(void);

struct S {
    u8 filler[0xcd8];
    void (*fp)(void);
    u8 f2[0xcf0 - 0xcdc];
    u32 v;
    u8 f3[0xd53 - 0xcf4];
    s8 b;
};

void sub_080F26B0(struct S *s)
{
    u32 v = s->v;
    if (v != 0) {
        if ((u8)sub_0824923C(s, v) != 0)
            s->b++;
    }
    if (s->b > 1) {
        s->b = 0;
        s->fp = sub_080F2700;
    }
}
