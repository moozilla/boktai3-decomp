// COMPILER: old_agbcc
#include "directional_force.h"
#define DirectionalForce_SetThreshold sub_080511EC

extern u16 gUnk_0200052C;

void DirectionalForce_SetThreshold(u16 v)
{
    gUnk_0200052C = v;
}
