#include "orbit_background.h"
void OrbitBackground_HoldCamera(void)
{
    if (gUnk_0200048C) {
        gUnk_0200048C->holdCamera = 1;
        gUnk_0200048C->blendFrame = 0;
    }
}
