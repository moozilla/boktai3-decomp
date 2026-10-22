#include "global.h"
extern u8 *gUnk_02000710;

s16 sub_0822E43C(void)
{
    u8 *g = gUnk_02000710;
    return *(s16 *)(g + *(s16 *)(g + 0x52) * 2 + 0x88);
}
