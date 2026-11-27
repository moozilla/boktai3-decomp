#include "objects/hazards/directional_force.h"
#include "engine/ram.h"

void DirectionalForce_Destroy(void)
{
    gPlayerDirectionLatch = 0;
    gPlayerBackgroundState = 0;
    gPlayerBackgroundMode = 0;
    gDirectionalForce = 0;
}
