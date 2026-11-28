#include "objects/hazards/directional_force.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void DirectionalForce_ConfigureFixed(s32, s32, s32, s32, s32);
void DirectionalForce_ConfigureFixedFromScript(void)
{
    s32 angle, baseMagnitude, magnitudeRange, baseDelay, delayRange;
    if (Script_SeekToKeyword('d'))
        angle = Script_GetValue();
    else
        angle = 0;
    if (Script_SeekToKeyword('p')) {
        baseMagnitude = Script_GetValue();
        magnitudeRange = Script_GetValue();
    } else {
        baseMagnitude = 3;
        magnitudeRange = 3;
    }
    if (Script_SeekToKeyword('t')) {
        baseDelay = Script_GetValue();
        delayRange = Script_GetValue();
    } else {
        baseDelay = 120;
        delayRange = 120;
    }
    DirectionalForce_ConfigureFixed(angle, baseMagnitude, magnitudeRange, baseDelay, delayRange);
}
