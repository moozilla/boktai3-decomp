#include "global.h"

extern const u8 gUnk_08E879D4[];
s32 LinkPlayer_GetElapsedActionTicks(u8 *);
void sub_08239D38(u8 *);

s32 sub_08239EB4(u8 *p, s32 want)
{
    s32 r = want;
    u8 *cnt = p + 0x5BB;
    if (*cnt != 0) {
        if (LinkPlayer_GetElapsedActionTicks(p) >= gUnk_08E879D4[p[0x5B9] - 1]) {
            sub_08239D38(p);
            {
                u8 v = p[0x4F4];
                p[0x4F5] = v;
            }
        }
        if (*cnt != 0) {
            if (p[0x5B9] == 1) {
                r = 3;
            } else if (p[0x5B9] == 2) {
                r = 4;
            } else if (p[0x5B9] == 3) {
                r = 5;
            }
        }
    }
    if (r == want) {
        u16 *t = (u16 *)(p + 0xACC);
        if (*t != 0) {
            if ((*t >> 2) & 1) {
                r = *(u16 *)(p + 0xACA);
            }
            (*t)--;
        }
    } else {
        *(u16 *)(p + 0xACA) = r;
        *(u16 *)(p + 0xACC) = 0x40;
    }
    return r;
}
