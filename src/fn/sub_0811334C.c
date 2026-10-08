#include "global.h"

extern u8 *gUnk_020004C4;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08113340(u8 *);
void sub_0821A0C0(u8 *);
void sub_081132E0(void);
void sub_08113310(void);

u8 *sub_0811334C(void)
{
    u8 *p;
    if (gUnk_020004C4 != 0)
        return gUnk_020004C4;
    p = sub_08219FBC(0xa, 0x818);
    if (p != 0) {
        sub_0821A04C(p, sub_081132E0, sub_08113310);
        if (sub_08113340(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
