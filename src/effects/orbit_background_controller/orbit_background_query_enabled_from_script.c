#include "effects/orbit_background_controller/orbit_background.h"
u32 OrbitBackground_IsEnabled(void);
u32 OrbitBackground_QueryEnabledFromScript(void)
{
    return OrbitBackground_IsEnabled();
}
