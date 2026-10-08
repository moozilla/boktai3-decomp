#include "global.h"

extern u8 *gUnk_020001CC;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0810E6E4(u8 *);
void sub_0821A0C0(u8 *);
void sub_0810E660(void);
void sub_0810E6B4(void);

u8 *sub_0810E8D0(void)
{
    u8 *p;
    if (gUnk_020001CC != 0)
        return gUnk_020001CC;
    p = sub_08219FBC(0xb, 0xd38);
    if (p != 0) {
        sub_0821A04C(p, sub_0810E660, sub_0810E6B4);
        if (sub_0810E6E4(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
