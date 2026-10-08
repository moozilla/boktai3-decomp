#include "global.h"

extern u8 *gUnk_0200012C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08121B08(u8 *);
void sub_0821A0C0(u8 *);
void sub_08121A78(void);
void sub_08121AC8(void);

u8 *sub_08121B40(void)
{
    u8 *p;
    if (gUnk_0200012C != 0)
        return gUnk_0200012C;
    p = sub_08219FBC(0xa, 0x2cc);
    if (p != 0) {
        sub_0821A04C(p, sub_08121A78, sub_08121AC8);
        if (sub_08121B08(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
