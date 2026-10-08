#include "global.h"
extern u8 *gUnk_02000140;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08122D58(u8 *);
void sub_0821A0C0(u8 *);
void sub_081228F0(void);
void sub_0812286C(void);
void sub_08122DA0(void)
{
    u8 *p;
    if (gUnk_02000140 == 0) {
        p = sub_08219FBC(10, 0x550);
        if (p != 0) {
            sub_0821A04C(p, sub_081228F0, sub_0812286C);
            if (sub_08122D58(p) < 0) sub_0821A0C0(p);
        }
    }
}
