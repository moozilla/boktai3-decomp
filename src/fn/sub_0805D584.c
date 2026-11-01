#include "scene_panel_subject_playback.h"
void ScenePanelSubject_TickResetDelay(struct ScenePanelSubject *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 1, owner->sequenceRow,
                     owner->sequenceVariant);
    ScenePanelSubject_StepPlayback(owner);
    owner->unknownE2--;
    if (!owner->unknownE2) {
        owner->positionReset = 0;
        owner->drawable.position = owner->home;
        ScenePanelSubject_SetState((void *)owner, 2);
    } else
        ScenePanelSubject_ResetPosition(owner);
}
