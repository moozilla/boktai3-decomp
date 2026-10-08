#include "global.h"

extern u16 gUnk_030042E0;
extern u16 *gUnk_030042E4;
u16 *sub_0821A520(u32, u32);
void sub_082171C0(u16 *, u32);

void sub_08215118(void)
{
    u16 *p = sub_0821A520(0x9B1B, 0xC5E9);
    gUnk_030042E0 = p[0];
    gUnk_030042E4 = p + 2;
    sub_082171C0(p + 2, 8);
}
