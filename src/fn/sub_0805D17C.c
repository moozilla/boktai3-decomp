#include "scene_panel_subject_playback.h"
void ScenePanelSubject_TickActive(struct ScenePanelSubject *owner)
{
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 3, owner->sequenceRow,
                     owner->sequenceVariant);
    ScenePanelSubject_StepPlayback(owner);
    if (owner->unknownEE)
        owner->paletteBank = 0x24E;
    else
        owner->paletteBank = 0x24D;
}
