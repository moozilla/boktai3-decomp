// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "effects/transient_sprite_effects/transient_sprite_effects.h"
/* TransientSpriteEffects_ResetInstance: Clears the global transient sprite effects instance pointer at gTransientSpriteEffects. */
#define TransientSpriteEffects_ResetInstance sub_0804732C

void sub_0804732C(void)
{
    gTransientSpriteEffects = 0;
}
