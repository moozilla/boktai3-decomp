#include "directional_force.h"
#define DirectionalForce_SetThresholdFromScript sub_080511F8
void DirectionalForce_SetThresholdFromScript(void)
{
    if (gUnk_02000114 && Script_SeekToKeyword('H'))
        gUnk_0200052C = Script_GetValue();
}
