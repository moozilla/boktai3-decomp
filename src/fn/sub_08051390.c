#include "directional_force.h"
#include "ram.h"

void DirectionalForce_Destroy(void)
{
    gPlayerDirectionLatch = 0;
    gPlayerBackgroundState = 0;
    gPlayerBackgroundMode = 0;
    gDirectionalForce = 0;
}
