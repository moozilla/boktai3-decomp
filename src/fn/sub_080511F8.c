#include "directional_force.h"
void DirectionalForce_SetThresholdFromScript(void)
{
    if (gDirectionalForce && Script_SeekToKeyword('H'))
        gPlayerDirectionLatch = Script_GetValue();
}
