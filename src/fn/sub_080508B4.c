#include "orbit_background.h"
u32 OrbitBackground_IsEnabled(void)
{
    u32 value = gUnk_02000528;
    u32 special = 0;
    if (value == 1)
        special = 1;
    if (!special) {
        if (gOrbitBackground)
            return gOrbitBackground->enabled;
    }
    return 0;
}
