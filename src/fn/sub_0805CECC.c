#include "scene_panel_subject_playback.h"
void ScenePanelSubject_TickIdle(struct ScenePanelSubject *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 1, owner->sequenceRow,
                     owner->sequenceVariant);
    ScenePanelSubject_StepPlayback(owner);
    owner->paletteBank = 0x24C;
}
