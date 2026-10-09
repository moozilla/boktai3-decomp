#include "global.h"

extern u8 *gUnk_0200004C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08011AD0(u8 *);
void sub_0821A0C0(u8 *);
void sub_08011A28(void);
void sub_08011AA4(void);

u8 *sub_08011C00(void)
{
    u8 *p;
    if (gUnk_0200004C != 0)
        return gUnk_0200004C;
    p = sub_08219FBC(0x8, 0xbd0);
    if (p != 0) {
        sub_0821A04C(p, sub_08011A28, sub_08011AA4);
        if (sub_08011AD0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
