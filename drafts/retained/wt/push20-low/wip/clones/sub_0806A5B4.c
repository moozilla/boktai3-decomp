#include "global.h"
extern u8 *gUnk_02000168;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0806A520(u8 *);
void sub_0821A0C0(u8 *);
void sub_08069FA4(void);
void sub_0806A138(void);
u8 *sub_0806A5B4(void)
{
    u8 *r;
    if (gUnk_02000168 != 0) return gUnk_02000168;
    r = sub_08219FBC(9, 0xef8);
    if (r) {
        sub_0821A04C(r, sub_08069FA4, sub_0806A138);
        if (sub_0806A520(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
