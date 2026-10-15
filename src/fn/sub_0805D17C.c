// MATCHING_CONCESSION: register-pinning
#include "scene_panel_subject_playback.h"
#define ScenePanelSubject_TickActive sub_0805D17C
void ScenePanelSubject_TickActive(struct ScenePanelSubjectView *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 3, owner->sequenceRow,
                     owner->sequenceVariant);
    ScenePanelSubject_StepPlayback(owner);
    if (owner->unknownEE)
        owner->paletteBank = 0x24E;
    else
        owner->paletteBank = 0x24D;
}
