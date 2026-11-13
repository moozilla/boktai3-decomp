#include "global.h"
#include "scene_departure_display.h"
#include "player_cue_sequence.h"

void ActorDisplay_UpdateForegroundSprite(struct DepartureActor *actor)
{
    struct DepartureDrawable *draw = &actor->foreground;
    u16 *origin, *screen, *shakeX;
    s32 *displacement;

    u32 x;
    u32 vertical;
    u32 y;
    u32 result;
    draw->ended = 0;
    screen = (u16 *)&actor->screen.x;
    origin = (u16 *)&actor->shakeStrength;
    x = *origin + 42;
    x += *screen;
    shakeX = (u16 *)&actor->shakeX;
    result = *shakeX + x;
    draw->sprite.position.x = result;
    y = actor->jitter;
    displacement = &actor->displacement;
    result = *displacement + 74;
    y += result;
    vertical = actor->dispersalTimer;
    vertical += y;
    vertical += *origin;
    result = (u16)actor->shakeZ + vertical;
    draw->sprite.position.y = result;
    if (actor->statePhase != 9) {
        if (actor->motionMode) {
            u16 *layer = &actor->alternateLayer;
            if (*(s16 *)layer)
                draw->sprite.flags |= 1;
            else
                draw->sprite.flags &= ~1;
        } else
            draw->sprite.flags &= ~1;
    }
    if (draw->mode != 1)
        draw->ended = PlayerCue_StepSequenceEnded(&draw->sprite, &draw->sequence);
}
