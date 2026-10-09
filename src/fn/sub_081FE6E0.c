#include "global.h"
extern u8 *gUnk_020005EC;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_081FE68C(u8 *);
void sub_0821A0C0(u8 *);
void sub_081FE2C4(void);
void sub_081FE644(void);
u8 *sub_081FE6E0(void)
{
    u8 *r;
    if (gUnk_020005EC != 0) return gUnk_020005EC;
    r = sub_08219FBC(10, 0xef8);
    if (r) {
        sub_0821A04C(r, sub_081FE2C4, sub_081FE644);
        if (sub_081FE68C(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
