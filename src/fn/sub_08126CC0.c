#include "global.h"
void AnimData_Load(void *, u32);
extern u8 *gUnk_0200015C;
u32 sub_08126CC0(u8 *p)
{
    AnimData_Load(p + 0x18, 0x8639);
    gUnk_0200015C = p;
    return 0;
}
