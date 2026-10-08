#include "global.h"

extern u8 *gUnk_020001B0;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08104E80(u8 *);
void sub_0821A0C0(u8 *);
void sub_08104D6C(void);
void sub_08104DE8(void);

u8 *sub_08104EB0(void)
{
    u8 *p;
    if (gUnk_020001B0 != 0)
        return gUnk_020001B0;
    p = sub_08219FBC(0xa, 0x630);
    if (p != 0) {
        sub_0821A04C(p, sub_08104D6C, sub_08104DE8);
        if (sub_08104E80(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
