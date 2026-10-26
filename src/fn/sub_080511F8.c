#include "directional_force.h"
void DirectionalForce_SetThresholdFromScript(void)
{
    if (gDirectionalForce && Script_SeekToKeyword('H'))
        gUnk_0200052C = Script_GetValue();
}
