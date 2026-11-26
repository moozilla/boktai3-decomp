#include "engine/shared/global.h"
#include "engine/shared/scene_departure_display.h"
#include "player/player_cue/player_cue_sequence.h"

void ActorDisplay_UpdateForegroundSprite(struct DepartureActor *actor)
{
    struct DepartureDrawable *draw = &actor->foreground;
    draw->ended = 0;
    draw->sprite.position.x = actor->screen.x + (s16)(actor->shakeStrength + 42) + actor->shakeX;
    draw->sprite.position.y = actor->jitter + (actor->displacement + 74) + actor->dispersalTimer
                              + actor->shakeStrength + actor->shakeZ;
    if (actor->statePhase != 9) {
        if (actor->motionMode && actor->alternateLayer)
            draw->sprite.flags |= 1;
        else
            draw->sprite.flags &= ~1;
    }
    if (draw->mode != 1)
        draw->ended = PlayerCue_StepSequenceEnded(&draw->sprite, &draw->sequence);
}
