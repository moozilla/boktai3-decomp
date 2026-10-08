#include "global.h"
void sub_082151E4(u8 *, u32);
extern u8 *gUnk_02000188;
u32 sub_0806CE98(u8 *p)
{
    *(u32 *)(p + 0x38) = 0;
    sub_082151E4(p + 0x18, 0xF422);
    gUnk_02000188 = p;
    return 0;
}
