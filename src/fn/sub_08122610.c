#include "global.h"

extern u8 *gUnk_0200013C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_081225F0(u8 *);
void sub_0821A0C0(u8 *);
void sub_0812254C(void);
void sub_081225A4(void);

u8 *sub_08122610(void)
{
    u8 *p;
    if (gUnk_0200013C != 0)
        return gUnk_0200013C;
    p = sub_08219FBC(0xa, 0x6f8);
    if (p != 0) {
        sub_0821A04C(p, sub_0812254C, sub_081225A4);
        if (sub_081225F0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
