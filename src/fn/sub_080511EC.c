// COMPILER: old_agbcc
#include "directional_force.h"
#include "ram.h"
/* DirectionalForce_SetThreshold: Stores the supplied threshold in the directional force global used by the corresponding script setter. */
#define DirectionalForce_SetThreshold sub_080511EC
#define DirectionalForce_SetThreshold sub_080511EC

void DirectionalForce_SetThreshold(u16 v)
{
    gPlayerDirectionLatch = v;
}
