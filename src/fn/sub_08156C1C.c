#include "global.h"

struct P08156C1C { u8 f0[0x34C]; s16 a[10]; u8 f360[0x54A - 0x360]; s8 f54A; };
s32 sub_08156BBC(void *);

void sub_08156C1C(struct P08156C1C *p)
{
    s32 i;
    s32 r;
    for (i = 9; i > 0; i--)
        p->a[i] = p->a[i - 1];
    r = sub_08156BBC(p);
    p->a[0] = r;
    if ((s16)r >= 0)
        { s32 t = r; t += p->f54A; p->a[0] = (t + 7) & 7; }
}
