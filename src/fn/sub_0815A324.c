#include "global.h"

extern u8 *gUnk_02000580;

void Player_ClearFlag740(void)
{
    u8 *g = gUnk_02000580;

    if (g != 0) {
        g[0x740] = 0;
    }
}
