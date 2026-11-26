#include "engine/shared/directional_force.h"
void DirectionalForce_ConfigureRandom(u32 baseMagnitude, u32 magnitudeRange, u32 baseDelay,
                                      u32 delayRange)
{
    struct DirectionalForce *force = gDirectionalForce;
    if (force) {
        force->mode = 1;
        force->baseMagnitude = baseMagnitude;
        force->magnitudeRange = magnitudeRange;
        force->baseDelay = baseDelay;
        force->delayRange = delayRange;
        force->delay = 0;
    }
}
