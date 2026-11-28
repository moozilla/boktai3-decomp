#include "objects/hazards/directional_force.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void DirectionalForce_ConfigureRandom(s32, s32, s32, s32);
void DirectionalForce_ConfigureRandomFromScript(void)
{
    s32 baseMagnitude, magnitudeRange, baseDelay, delayRange;
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
    DirectionalForce_ConfigureRandom(baseMagnitude, magnitudeRange, baseDelay, delayRange);
}
