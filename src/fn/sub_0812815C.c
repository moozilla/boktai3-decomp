#include "global.h"

extern u8 *gUnk_0200016C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08128124(u8 *);
void sub_0821A0C0(u8 *);
void sub_0812808C(void);
void sub_081280E4(void);

u8 *sub_0812815C(void)
{
    u8 *p;
    if (gUnk_0200016C != 0)
        return gUnk_0200016C;
    p = sub_08219FBC(0xa, 0x7bc);
    if (p != 0) {
        sub_0821A04C(p, sub_0812808C, sub_081280E4);
        if (sub_08128124(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
