#include "global.h"
extern const u16 gUnk_08605F30[];
struct Q { u8 filler[8]; u16 h8; u8 f2[4]; u16 hE; };
struct R { u8 f[4]; u8 *p; u8 f2[0x2c2 - 8]; u16 h2c2; u8 f3[0x2d0 - 0x2c4]; u32 flags; };
s32 sub_080A4940(struct R *s) {
    struct Q *q = (struct Q *)(s->p + 0x48);
    if (q->hE == 0) {
        u32 c = 0x10000;
        s->flags |= c;
        return gUnk_08605F30[q->h8 & 1];
    }
    return 0;
}
