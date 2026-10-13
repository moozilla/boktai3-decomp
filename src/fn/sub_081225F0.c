#include "global.h"
void AnimData_Load(void *, u32);
extern u8 *gUnk_0200013C;
u32 sub_081225F0(u8 *p)
{
    *(u32 *)(p + 0x34) = 0;
    gUnk_0200013C = p;
    AnimData_Load(p + 0x18, 0x84EE);
    return 0;
}
