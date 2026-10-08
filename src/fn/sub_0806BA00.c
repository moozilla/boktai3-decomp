#include "global.h"

extern u8 *gUnk_02000178;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0806B9F0(u8 *);
void sub_0821A0C0(u8 *);
void sub_0806B8DC(void);
void sub_0806B96C(void);

u8 *sub_0806BA00(void)
{
    u8 *p;
    if (gUnk_02000178 != 0)
        return gUnk_02000178;
    p = sub_08219FBC(10, 0xBBC);
    if (p != 0) {
        sub_0821A04C(p, sub_0806B8DC, sub_0806B96C);
        if (sub_0806B9F0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
