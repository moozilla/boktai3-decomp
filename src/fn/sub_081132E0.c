#include "global.h"
struct E { u8 f[0x19]; u8 b; };
void sub_0824923C(void *, u32);
s32 sub_081132E0(u8 *s)
{
    s32 i;
    struct E *p;
    i = 0;
    p = (struct E *)(s + 0x18);
    do {
        s32 off = i << 5;
        u8 *q;
        if (p->b != 0) {
            q = s + 0x34;
            q = q + off;
            sub_0824923C(p, *(u32 *)q);
        }
        p = (struct E *)((u8 *)p + 0x20);
        i++;
    } while (i <= 0x3f);
    return 0;
}
