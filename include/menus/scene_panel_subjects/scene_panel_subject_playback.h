#ifndef GUARD_SCENE_PANEL_SUBJECT_PLAYBACK_H
#define GUARD_SCENE_PANEL_SUBJECT_PLAYBACK_H
#include "engine/shared/scene_panel_subject.h"
#include "player/player_cue/player_cue_sequence.h"
static inline u32 ScenePanelSubject_StepPlayback(struct ScenePanelSubject *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->drawable, &owner->sequence);
}
#endif
