// MATCHING_CONCESSION: register-pinning
#include "scene_panel_subject_playback.h"
#define ScenePanelSubject_TickAppearing sub_0805CD64
void ScenePanelSubject_TickAppearing(struct ScenePanelSubjectView *owner)
{
    if (owner->appearDelay)
        owner->appearDelay--;
    else {
        owner->drawable.flags &= ~1;
        owner->collider.flags &= ~4;
        owner->drawable.priority = 2;
        Anim_SetSequence(&owner->sequence, owner->sequenceResource, 0, owner->sequenceRow,
                         owner->sequenceVariant);
        if (ScenePanelSubject_StepPlayback(owner)) {
            Collider_Refresh(&owner->collider);
            sub_0805CB88((void *)owner, 2);
        }
    }
}
