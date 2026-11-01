#ifndef GUARD_ROUTE_SPRITE_PLAYBACK_H
#define GUARD_ROUTE_SPRITE_PLAYBACK_H
#include "route_sprite_actor.h"
#include "player_cue_sequence.h"
static inline u32 RouteSpriteActor_StepPlayback(struct RouteSpriteActor *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}
#endif
