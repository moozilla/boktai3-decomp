#ifndef GUARD_PILE_DRIVER_SUBJECT_PLAYBACK_H
#define GUARD_PILE_DRIVER_SUBJECT_PLAYBACK_H
#include "objects/pile_driver/pile_driver_subject.h"
#include "player/cutscene/player_cue_sequence.h"
static inline u32 PileDriverSubject_StepPlayback(struct PileDriverSubject *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->drawable, &owner->sequence);
}
#endif
