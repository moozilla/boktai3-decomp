#include "orbit_background.h"
void OrbitBackground_Enable(void)
{
    struct OrbitBackground *background = gUnk_0200048C;
    if (background && !background->enabled) {
        OrbitBackground_ResetOrbit(background);
        background->enabled = 1;
    }
}
