#include "global.h"

extern u32 gUnk_03004BC0[];
extern u16 gUnk_03004BE0[];
extern u16 gUnk_03004C08[];

void sub_08215A48(u32 i, u32 a, u16 b, u16 c)
{
    gUnk_03004BC0[i] = a;
    gUnk_03004BE0[i] = c;
    gUnk_03004C08[i] = b;
}
