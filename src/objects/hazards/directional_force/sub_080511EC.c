// COMPILER: old_agbcc
#include "objects/hazards/directional_force.h"
#include "engine/ram.h"
/* DirectionalForce_SetThreshold: Stores the supplied threshold in the directional force global used by the corresponding script setter. */

void DirectionalForce_SetThreshold(u16 v)
{
    gPlayerDirectionLatch = v;
}
