#include "global.h"
struct A { u8 p[0x10]; void *f10; };
extern struct A *gUnk_030052F4;
void *sub_0821A520(u32, u16);
u32 sub_0821E594(u16 v)
{
    gUnk_030052F4->f10 = sub_0821A520(0xD4FB, v);
    return 0;
}
