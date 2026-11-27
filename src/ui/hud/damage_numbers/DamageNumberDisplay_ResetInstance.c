// COMPILER: old_agbcc
#include "global.h"
/* DamageNumberDisplay_ResetInstance: Clears the global damage number display instance pointer at gDamageNumberDisplay. */

extern u32 gDamageNumberDisplay;

void DamageNumberDisplay_ResetInstance(void)
{
    gDamageNumberDisplay = 0;
}
