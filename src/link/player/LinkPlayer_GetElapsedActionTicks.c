#include "global.h"
#include "player/player.h"

/* The link player counts action ticks in facingChangeEffects. */
s32 LinkPlayer_GetElapsedActionTicks(struct Player *p)
{
    return (p->facingChangeEffects - p->savedEffectTick + 0x100) & 0xFF;
}
