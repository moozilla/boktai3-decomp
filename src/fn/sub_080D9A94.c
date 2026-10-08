#include "global.h"

s32 Mod(s32, s32);
extern const u16 gUnk_086061B8[];

struct Q { u8 filler[8]; u16 h8; u8 f2[4]; u16 hE; };
struct R { u8 f[4]; u8 *p; u8 f2[0x2c2 - 8]; u16 h2c2; u8 f3[0x2d0 - 0x2c4]; u32 flags; };

s32 sub_080D9A94(struct R *s)
{
    struct Q *q = (struct Q *)(s->p + 0x48);
    if (q->hE == 0) {
        s32 m = Mod(q->h8, 4);
        u32 c = 0x10000;
        s->flags |= c;
        if (m != 0) {
            u16 h = s->h2c2;
            s32 k = 1;
            if (h <= 0x1f)
                k = 2;
            return gUnk_086061B8[m] * k;
        }
    }
    return 0;
}
