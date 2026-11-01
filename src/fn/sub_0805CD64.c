#include "scene_panel_subject_playback.h"
void ScenePanelSubject_TickAppearing(struct ScenePanelSubject *owner)
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
            ScenePanelSubject_SetState((void *)owner, 2);
        }
    }
}
