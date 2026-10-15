#include "orbit_background.h"
#define OrbitBackground_Disable sub_0805089C
void OrbitBackground_Disable(void)
{
    struct OrbitBackground *background = gUnk_0200048C;
    if (background)
        background->enabled = 0;
}
