#include "effects/orbit_background_controller/orbit_background.h"
void OrbitBackground_Enable(void)
{
    struct OrbitBackground *background = gOrbitBackground;
    if (background && !background->enabled) {
        OrbitBackground_ResetOrbit(background);
        background->enabled = 1;
    }
}
