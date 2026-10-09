#include "global.h"

extern u8 *gUnk_02000074;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_080471CC(u8 *);
void sub_0821A0C0(u8 *);
void sub_08047168(void);
void sub_080471A8(void);

u8 *sub_08047258(void)
{
    u8 *p;
    if (gUnk_02000074 != 0)
        return gUnk_02000074;
    p = sub_08219FBC(0x9, 0x19c);
    if (p != 0) {
        sub_0821A04C(p, sub_08047168, sub_080471A8);
        if (sub_080471CC(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
