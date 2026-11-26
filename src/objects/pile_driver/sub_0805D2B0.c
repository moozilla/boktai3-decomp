#include "objects/pile_driver/scene_panel_subject_playback.h"
void ScenePanelSubject_TickDeparting(struct ScenePanelSubject *owner)
{
    if (owner->kind) {
        if (owner->sequence.index == 5 && owner->sequence.elapsed == 0)
            owner->paletteBank = 0x24D;
        else if (owner->sequence.index == 6 && owner->sequence.elapsed == 0)
            owner->paletteBank = 0x24C;
        Anim_SetSequence(&owner->sequence, owner->sequenceResource, 4, owner->sequenceRow,
                         owner->sequenceVariant);
        if (ScenePanelSubject_StepPlayback(owner)) {
            owner->contactProgress = 0;
            owner->unknownE2 = 90;
            ScenePanelSubject_SetState((void *)owner, 6);
        }
    } else {
        if (owner->sequence.index == 5 && owner->sequence.elapsed == 0)
            owner->paletteBank = 0x24C;
        Anim_SetSequence(&owner->sequence, owner->sequenceResource, 4, owner->sequenceRow,
                         owner->sequenceVariant);
        if (ScenePanelSubject_StepPlayback(owner)) {
            owner->contactProgress = 0;
            ScenePanelSubject_SetState((void *)owner, 2);
        }
    }
}
