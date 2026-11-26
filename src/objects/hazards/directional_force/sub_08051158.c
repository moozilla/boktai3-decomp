#include "objects/hazards/directional_force.h"
void DirectionalForce_ConfigureFixed(u32 angle, u32 baseMagnitude, u32 magnitudeRange,
                                     u32 baseDelay, u32 delayRange)
{
    struct DirectionalForce *force = gDirectionalForce;
    if (force) {
        force->mode = 2;
        force->targetAngle = angle;
        force->baseMagnitude = baseMagnitude;
        force->magnitudeRange = magnitudeRange;
        force->baseDelay = baseDelay;
        force->delayRange = delayRange;
        force->delay = 0;
    }
}
