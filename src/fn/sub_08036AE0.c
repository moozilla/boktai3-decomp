#include "global.h"

struct S { u8 pad0[56]; u16 f2; u32 f4; u32 f8; };
extern struct S *gUnk_02000484;

u16 sub_08036AE0(void)
{
    if (gUnk_02000484 == 0)
        return 0;
    return gUnk_02000484->f2;
}
