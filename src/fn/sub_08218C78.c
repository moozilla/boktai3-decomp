#include "global.h"

struct S { u8 pad0[2]; u16 f2; u32 f4; u32 f8; };
extern struct S *gUnk_03001664;

u32 sub_08218C78(void)
{
    if (gUnk_03001664 == 0)
        return 0;
    return gUnk_03001664->f4;
}
