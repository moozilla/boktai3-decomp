#ifndef GUARD_ROAMING_OBJECT_PLAYBACK_H
#define GUARD_ROAMING_OBJECT_PLAYBACK_H
#include "roaming_object.h"
#include "player_cue_sequence.h"
static inline u32 RoamingObject_StepPlayback(struct RoamingObject *owner,
                                             struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, state);
}

#endif
