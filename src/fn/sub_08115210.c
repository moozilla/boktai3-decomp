#include "global.h"

extern u8 *gUnk_020001DC;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_081151F8(u8 *);
void sub_0821A0C0(u8 *);
void sub_08115104(void);
void sub_08115190(void);

u8 *sub_08115210(void)
{
    u8 *p;
    if (gUnk_020001DC != 0)
        return gUnk_020001DC;
    p = sub_08219FBC(0xa, 0xaa0);
    if (p != 0) {
        sub_0821A04C(p, sub_08115104, sub_08115190);
        if (sub_081151F8(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
