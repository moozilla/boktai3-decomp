#include "global.h"

extern u8 *gUnk_02000580;

void Player_SetTransitionMode(s32 v)
{
    u8 *g = gUnk_02000580;

    if (g != 0) {
        g[0xAC9] = v;
    }
}
