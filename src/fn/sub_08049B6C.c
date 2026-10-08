#include "global.h"
extern u8 *gUnk_02000488;
u8 *sub_08049B6C(void)
{
    u8 *p = gUnk_02000488;
    if (!p) return 0;
    return p + 0x50;
}
