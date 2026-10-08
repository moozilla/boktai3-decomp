#include "global.h"
extern const u32 gUnk_0861074C[];
void sub_0824923C(u8 *, u32);
u32 sub_08126568(u8 *p)
{
    const u32 *t = gUnk_0861074C;
    u8 *r = p + 0x74;
    u8 *q = p + 0x1c;
    s32 n = 0x2f;
    do {
        if (*(s8 *)(r + 0x14) >= 0)
            sub_0824923C(q, t[*r]);
        r += 0x70;
        q += 0x70;
        n--;
    } while (n >= 0);
    return 0;
}
