#include "global.h"
struct A { u8 p[0xc]; void *fc; };
extern struct A *gUnk_030052F4;
extern u32 gUnk_03001684;
void *sub_0821A520(u32, u16);
u32 sub_0821DDC8(u16 v)
{
    gUnk_030052F4->fc = sub_0821A520(0xDCFB, v);
    return gUnk_03001684 = 0;
}
