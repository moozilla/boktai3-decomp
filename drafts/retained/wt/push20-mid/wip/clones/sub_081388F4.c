#include "global.h"
struct E { u8 f[0x4]; u8 b; u8 pad[6]; };
extern u32 gUnk_020004C0;
void sub_08214514(void *);
s32 sub_081388F4(u8 *s)
{
    s32 i;
    u32 z;
    struct E *p = (struct E *)(s + 0x34);
    i = 0x3;
    do {
        if (p->b != 0)
            sub_08214514(p);
        p++;
    } while (--i >= 0);
    z = 0;
    gUnk_020004C0 = z;
    return 0;
}
