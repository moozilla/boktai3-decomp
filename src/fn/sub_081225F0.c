#include "global.h"
void sub_082151E4(void *, u32);
extern u8 *gUnk_0200013C;
u32 sub_081225F0(u8 *p)
{
    *(u32 *)(p + 0x34) = 0;
    gUnk_0200013C = p;
    sub_082151E4(p + 0x18, 0x84EE);
    return 0;
}
