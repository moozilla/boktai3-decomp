// MATCHING_CONCESSION: register-pinning
#include "scene_panel_subject_playback.h"
#define ScenePanelSubject_TickResetDelay sub_0805D584
void ScenePanelSubject_TickResetDelay(struct ScenePanelSubjectView *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 1, owner->sequenceRow,
                     owner->sequenceVariant);
    ScenePanelSubject_StepPlayback(owner);
    owner->unknownE2--;
    if (!owner->unknownE2) {
        owner->positionReset = 0;
        owner->drawable.position = owner->home;
        sub_0805CB88((void *)owner, 2);
    } else
        sub_0805CBD4(owner);
}
