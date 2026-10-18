// COMPILER: old_agbcc
#include "global.h"
/* TransientSpriteEffects_ResetInstance: Clears the global transient sprite effects instance pointer at gUnk_020000A0. */
#define TransientSpriteEffects_ResetInstance sub_0804732C

extern u32 gUnk_020000A0;

void sub_0804732C(void)
{
    gUnk_020000A0 = 0;
}
