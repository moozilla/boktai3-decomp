#include "orbit_background.h"
#define OrbitBackground_IsEnabled sub_080508B4
u32 OrbitBackground_IsEnabled(void)
{
    u32 value = gUnk_02000528;
    u32 special = 0;
    if (value == 1)
        special = 1;
    if (!special) {
        if (gUnk_0200048C)
            return gUnk_0200048C->enabled;
    }
    return 0;
}
