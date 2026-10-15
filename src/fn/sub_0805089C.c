#include "orbit_background.h"
void OrbitBackground_Disable(void)
{
    struct OrbitBackground *background = gUnk_0200048C;
    if (background)
        background->enabled = 0;
}
