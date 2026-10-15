// MATCHING_CONCESSION: register-pinning
#include "scene_panel_subject_playback.h"
#define ScenePanelSubject_TickActivating sub_0805CFE8
void ScenePanelSubject_TickActivating(struct ScenePanelSubjectView *owner)
{
    if (owner->kind) {
        const volatile u16 *source = &owner->sequence.index;
        register u32 first asm("r1") = *source;
        const volatile u16 *index = source;
        asm("" ::"r"(first), "r"(index));
        if (first == 5) {
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
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 2, owner->sequenceRow,
                     owner->sequenceVariant);
    if (ScenePanelSubject_StepPlayback(owner)) {
        owner->changed = 1;
        sub_0805CB88((void *)owner, 4);
    }
}
