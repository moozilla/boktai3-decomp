// COMPILER: old_agbcc
#include "global.h"
/* DamageNumberDisplay_ResetInstance: Clears the global damage number display instance pointer at gUnk_02000074. */
#define DamageNumberDisplay_ResetInstance sub_08046B28

extern u32 gUnk_02000074;

void sub_08046B28(void)
{
    gUnk_02000074 = 0;
}
