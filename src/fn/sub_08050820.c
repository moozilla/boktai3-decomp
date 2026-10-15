#include "orbit_background.h"
#define OrbitBackground_HoldCamera sub_08050820
void OrbitBackground_HoldCamera(void)
{
    if (gUnk_0200048C) {
        gUnk_0200048C->holdCamera = 1;
        gUnk_0200048C->blendFrame = 0;
    }
}
