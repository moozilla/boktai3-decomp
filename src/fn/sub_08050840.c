#include "orbit_background.h"
void OrbitBackground_ReleaseCamera(void)
{
    if (gOrbitBackground) {
        gOrbitBackground->holdCamera = 0;
        gOrbitBackground->blendFrame = 8;
    }
}
