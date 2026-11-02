#include "scene_departure_display.h"

static inline u32 StepPlayback(struct DepartureDrawable *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}
void ActorDisplay_UpdateAccentSprite(struct DepartureActor *actor)
{
    struct DepartureDrawable *draw = &actor->accent;
    u16 *origin;
    u32 zero;
    u16 *screen = (u16 *)&actor->screen.x;
    s32 offset = draw->ended + 38;
    u16 x = *screen;
    origin = (u16 *)&actor->shakeStrength;
    x = x - offset + *origin;
    zero = 0;
    draw->sprite.position.x = x;
    draw->sprite.position.y = *origin + 149;
    if (!actor->blinkActive)
        draw->sprite.flags &= ~1;
    if (++draw->ended > 2)
        draw->ended = zero;
    if (draw->mode == 0) {
        if (StepPlayback(draw) == 1)
            draw->ended = 0;
    } else
        draw->ended = zero;
}
