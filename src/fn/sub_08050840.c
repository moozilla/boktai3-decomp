#include "orbit_background.h"
void OrbitBackground_ReleaseCamera(void)
{
    if (gUnk_0200048C) {
        gUnk_0200048C->holdCamera = 0;
        gUnk_0200048C->blendFrame = 8;
    }
}
