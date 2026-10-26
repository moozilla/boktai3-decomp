#include "orbit_background.h"
void OrbitBackground_Disable(void)
{
    struct OrbitBackground *background = gOrbitBackground;
    if (background)
        background->enabled = 0;
}
