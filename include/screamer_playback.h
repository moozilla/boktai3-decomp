#ifndef GUARD_SCREAMER_PLAYBACK_H
#define GUARD_SCREAMER_PLAYBACK_H
#include "screamer_state.h"
#include "player_cue_sequence.h"
static inline u32 Screamer_StepAnimation(struct ScreamerState *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}

#endif
