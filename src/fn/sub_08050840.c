#include "orbit_background.h"
#define OrbitBackground_ReleaseCamera sub_08050840
void OrbitBackground_ReleaseCamera(void)
{
    if (gUnk_0200048C) {
        gUnk_0200048C->holdCamera = 0;
        gUnk_0200048C->blendFrame = 8;
    }
}
