#include "orbit_background.h"
s32 OrbitBackground_ClearSingleton(void)
{
    gOrbitBackground = 0;
    return 0;
}
