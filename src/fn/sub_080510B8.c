#include "directional_force.h"
void DirectionalForce_Stop(void)
{
    struct DirectionalForce *force = gDirectionalForce;
    if (force) {
        force->mode = 0;
        force->targetAngle = 0;
        force->targetMagnitude = 0;
        force->delay = 0;
    }
}
