#include "global.h"

extern const u32 gUnk_08605B18[];
void sub_08064C48(u32, u32, u32, s32 *, s32 *, u32);
void sub_08217EC4(u8 *, s32, s32);
void sub_0824923C(u8 *, u32);

void sub_08065B4C(u8 *p)
{
    u8 *e = *(u8 **)(p + 0x134);
    if (e != NULL) {
        s32 a, b;
        if (p[0x33] == 0xff) {
            u32 x = *(u32 *)(e + 0x34);
            u32 y = *(u32 *)(e + 0x38);
            *(u32 *)(p + 0x1c) = x;
            *(u32 *)(p + 0x20) = y;
        } else {
            u8 *m = p + 0x3f;
            s32 i;
            u8 *q;
            sub_08064C48(*m, p[0x33], e[9], &a, &b, *(u32 *)(p + 0x5c));
            if ((u8)(*m - 2) <= 1) {
                i = 0;
                q = p + 0x3c;
                m += 0x4d;
                for (; i <= 2; i++) {
                    if ((*q >> i) & 1)
                        sub_08217EC4(m, a, b);
                    m += 0x28;
                }
            } else {
                s32 x = a;
                s32 z = 0;
                *(u16 *)(p + 0x1c) = x;
                *(u16 *)(p + 0x1e) = z;
                *(u16 *)(p + 0x20) = b;
            }
        }
    }
    sub_0824923C(p, gUnk_08605B18[p[0x3f]]);
}
