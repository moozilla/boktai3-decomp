#include "global.h"
extern u8 *gUnk_02000130;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08068258(u8 *, u32);
void sub_0821A0C0(u8 *);
void sub_08068140(void);
void sub_080681DC(void);
u8 *sub_08068270(void)
{
    u8 *r;
    if (gUnk_02000130 != 0) return gUnk_02000130;
    r = sub_08219FBC(9, 0xA28);
    gUnk_02000130 = r;
    if (r) {
        sub_0821A04C(r, sub_08068140, sub_080681DC);
        if (sub_08068258(r, 0) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
