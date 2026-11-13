#include "scene_panel_subject_playback.h"
void ScenePanelSubject_TickActivating(struct ScenePanelSubject *owner)
{
    union {
        u32 first;
        const void *resource;
    } work;
    if (owner->kind) {
        const u16 *source = &owner->sequence.index;
        const u16 *index;
        work.first = *source;
        index = source;
        if (work.first == 5) {
            source = &owner->sequence.elapsed;
            if (*source == 0)
                goto activationPalette;
        }
        if (*index > 5) {
            if (owner->unknownEE)
                owner->paletteBank = 0x24E;
            else
                goto activationPalette;
        }
    } else if (owner->sequence.index == 5 && owner->sequence.elapsed == 0) {
    activationPalette:
        owner->paletteBank = 0x24D;
    }
    {
        struct DisplaySequenceState *sequence = &owner->sequence;
        work.resource = owner->sequenceResource;
        Anim_SetSequence(sequence, work.resource, 2, owner->sequenceRow, owner->sequenceVariant);
    }
    if (ScenePanelSubject_StepPlayback(owner)) {
        owner->changed = 1;
        ScenePanelSubject_SetState((void *)owner, 4);
    }
}
