#include "global.h"
struct G0894 { u8 pad[0x32]; s16 y; };
extern struct G0894 *gUnk_02000580;
static inline u8 test0894(u8 *p) { u8 *q = p + 0xDC; u32 mask = 2; if (q[0xF] & mask) return 1; return 0; }
s32 sub_080A0894(u8 *s) {
    s32 d;
    u8 *p;
    if (!test0894(s)) goto zero;
    d = *(s16 *)(s + 0x5E) - gUnk_02000580->y;
    if (d < 0) d = -d;
    if (d > 0x200) goto zero;
    if (*(u32 *)(s + 0xF0) < *(u32 *)(s + 0xF8)) goto yes;
zero:
    return 0;
yes:
    {
        u32 mask = 0x80;
        u16 *flags = (u16 *)(s + 0x2D6);
        mask |= *flags;
        *flags = mask;
    }
    p = s + 0xDC;
    {
        u32 mask = 0x10;
        mask |= p[0xF];
        p[0xF] = mask;
    }
    return 1;
}
