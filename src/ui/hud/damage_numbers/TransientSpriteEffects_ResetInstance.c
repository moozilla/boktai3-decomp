// COMPILER: old_agbcc
#include "global.h"
#include "effects/sprites/transient_sprite_effects.h"
/* TransientSpriteEffects_ResetInstance: Clears the global transient sprite effects instance pointer at gTransientSpriteEffects. */

void TransientSpriteEffects_ResetInstance(void)
{
    gTransientSpriteEffects = 0;
}
