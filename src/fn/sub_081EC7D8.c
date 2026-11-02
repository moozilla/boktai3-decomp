// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_departure_display.h"
#include "player_cue_sequence.h"

void ActorDisplay_UpdateForegroundSprite(void *input)
{
    struct DepartureActor *actor = input;
    struct DepartureDrawable *draw = &actor->foreground;
    u16 *origin;
    register u16 *screen;
    register u32 x asm("r1");
    u32 y;
    u32 result;
    draw->ended = 0;
    screen = (u16 *)&actor->screen.x;
    origin = (u16 *)&actor->shakeStrength;
    x = *origin + 42;
    x += *screen;
    screen = (u16 *)&actor->shakeX;
    result = *screen + x;
    draw->sprite.position.x = result;
    y = actor->jitter;
    screen = (u16 *)&actor->displacement;
    result = *(s32 *)screen + 74;
    y += result;
    x = actor->dispersalTimer;
    x += y;
    x += *origin;
    result = (u16)actor->shakeZ + x;
    draw->sprite.position.y = result;
    if (actor->statePhase != 9) {
        if (actor->motionMode) {
            screen = &actor->alternateLayer;
            if (*(s16 *)screen)
                draw->sprite.flags |= 1;
            else
                draw->sprite.flags &= ~1;
        } else
            draw->sprite.flags &= ~1;
    }
    if (draw->mode != 1)
        draw->ended = PlayerCue_StepSequenceEnded(&draw->sprite, &draw->sequence);
}
