// MATCHING_CONCESSION: register-pinning
#include "scene_panel_subject_playback.h"
#define ScenePanelSubject_TickIdle sub_0805CECC
void ScenePanelSubject_TickIdle(struct ScenePanelSubjectView *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 1, owner->sequenceRow,
                     owner->sequenceVariant);
    ScenePanelSubject_StepPlayback(owner);
    owner->paletteBank = 0x24C;
}
