#include "global.h"

extern u8 *gUnk_02000214;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08178FC8(u8 *, u32);
void sub_0821A0C0(u8 *);
void sub_08178B98(void);
void sub_08178BCC(void);

u8 *sub_0817923C(u32 a)
{
    u8 *p;
    if (gUnk_02000214 != 0)
        return gUnk_02000214;
    p = sub_08219FBC(0xc, 0x4DA0);
    if (p != 0) {
        sub_0821A04C(p, sub_08178B98, sub_08178BCC);
        *(u16 *)(p + 0xA5A) = a;
        if (sub_08178FC8(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
