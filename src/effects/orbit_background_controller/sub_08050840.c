#include "effects/orbit_background_controller/orbit_background.h"
void OrbitBackground_ReleaseCamera(void)
{
    if (gOrbitBackground) {
        gOrbitBackground->holdCamera = 0;
        gOrbitBackground->blendFrame = 8;
    }
}
