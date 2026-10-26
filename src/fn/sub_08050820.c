#include "orbit_background.h"
void OrbitBackground_HoldCamera(void)
{
    if (gOrbitBackground) {
        gOrbitBackground->holdCamera = 1;
        gOrbitBackground->blendFrame = 0;
    }
}
