#include "objects/pile_driver/scene_panel_subject_tail.h"
void PileDriverSubject_UpdateTimedEffect(struct ScenePanelSubject *subject)
{
    if (subject->byteTimer)
        subject->byteTimer--;
    if (subject->countdown == 0) {
        if (subject->stateFrame == 0)
            EffectGroup_Reset(subject->effects);
        subject->stateFrame++;
        if (subject->stateFrame > 15) {
            subject->drawable.position = subject->home;
            subject->paletteBank = 0x24E;
            PileDriverSubject_SetState((void *)subject, 4);
            subject->collider.cost = 0;
            subject->stateFrame = 0;
        } else
            subject->paletteBank = subject->basePaletteBank - 1;
    } else {
        subject->paletteBank = subject->basePaletteBank;
        subject->countdown--;
        if (subject->countdown < subject->nextThreshold) {
            EffectGroup_FadeFirstActive(subject->effects);
            subject->nextThreshold -= 250;
        }
    }
}
