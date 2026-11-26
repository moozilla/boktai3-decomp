// COMPILER: old_agbcc
#include "engine/shared/global.h"
/* DamageNumberDisplay_ResetInstance: Clears the global damage number display instance pointer at gDamageNumberDisplay. */
#define DamageNumberDisplay_ResetInstance sub_08046B28

extern u32 gDamageNumberDisplay;

void sub_08046B28(void)
{
    gDamageNumberDisplay = 0;
}
