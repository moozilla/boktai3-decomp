#include "global.h"

extern u8 *gUnk_020004BC;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08112A90(u8 *);
void sub_0821A0C0(u8 *);
void sub_081128F8(void);
void sub_08112A70(void);

u8 *sub_08112B5C(void)
{
    u8 *p;
    if (gUnk_020004BC != 0)
        return gUnk_020004BC;
    p = sub_08219FBC(0xa, 0x31c);
    if (p != 0) {
        sub_0821A04C(p, sub_081128F8, sub_08112A70);
        if (sub_08112A90(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
