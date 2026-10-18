#include "global.h"
extern u8 *gUnk_02000214;
void Hud_StartMessageDisplayTimer(void)
{
    u8 *g = gUnk_02000214;
    if (g != 0)
        g[0xa6d] = 0x5a;
}
