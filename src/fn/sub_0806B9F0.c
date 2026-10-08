#include "global.h"
extern u8 *gUnk_02000178;
u32 sub_0806B9F0(u8 *p)
{
    *(u32 *)(p + 0x18) = 0;
    gUnk_02000178 = p;
    return 0;
}
