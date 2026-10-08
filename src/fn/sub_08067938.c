#include "global.h"
extern u8 *gUnk_02000128;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08067900(u8 *);
void sub_0821A0C0(u8 *);
void sub_08067854(void);
void sub_080678B0(void);
u8 *sub_08067938(void)
{
    u8 *r;
    if (gUnk_02000128 != 0) return gUnk_02000128;
    r = sub_08219FBC(10, 0x59c);
    if (r) {
        sub_0821A04C(r, sub_08067854, sub_080678B0);
        if (sub_08067900(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
