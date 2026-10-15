#include "directional_force.h"
#define DirectionalForce_Stop sub_080510B8
void DirectionalForce_Stop(void)
{
    struct DirectionalForce *force = gUnk_02000114;
    if (force) {
        force->mode = 0;
        force->targetAngle = 0;
        force->targetMagnitude = 0;
        force->delay = 0;
    }
}
