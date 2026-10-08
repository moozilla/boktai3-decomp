#include "global.h"

struct S { u8 pad0[24]; u16 f2; u32 f4; u32 f8; };
extern struct S *gUnk_0200049C;

u16 sub_0805751C(void)
{
    if (gUnk_0200049C == 0)
        return 0;
    return gUnk_0200049C->f2;
}
