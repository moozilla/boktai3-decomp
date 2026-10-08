#include "global.h"

extern u8 *gUnk_02000120;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08057480(u8 *);
void sub_0821A0C0(u8 *);
void sub_0805732C(void);
void sub_080573C8(void);

u8 *sub_080574D0(void)
{
    u8 *p;
    if (gUnk_02000120 != 0)
        return gUnk_02000120;
    p = sub_08219FBC(0x9, 0x260);
    if (p != 0) {
        sub_0821A04C(p, sub_0805732C, sub_080573C8);
        if (sub_08057480(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
