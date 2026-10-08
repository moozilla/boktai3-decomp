#include "global.h"

extern u8 *gUnk_02000150;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_081260BC(u8 *);
void sub_0821A0C0(u8 *);
void sub_08126000(void);
void sub_08126074(void);

u8 *sub_081260E8(void)
{
    u8 *p;
    if (gUnk_02000150 != 0)
        return gUnk_02000150;
    p = sub_08219FBC(0xa, 0x6a0);
    if (p != 0) {
        sub_0821A04C(p, sub_08126000, sub_08126074);
        if (sub_081260BC(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
