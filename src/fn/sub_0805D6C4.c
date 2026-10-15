#include "scene_panel_subject_tail.h"
#define ScenePanelSubject_UpdateTimedEffect sub_0805D6C4
void ScenePanelSubject_UpdateTimedEffect(struct ScenePanelSubjectView *subject)
{
    if (subject->byteTimer)
        subject->byteTimer--;
    if (subject->countdown == 0) {
        if (subject->stateFrame == 0)
            sub_08231568(subject->effects);
        subject->stateFrame++;
        if (subject->stateFrame > 15) {
            subject->drawable.position = subject->home;
            subject->paletteBank = 0x24E;
            sub_0805CB88((void *)subject, 4);
            subject->collider.cost = 0;
            subject->stateFrame = 0;
        } else
            subject->paletteBank = subject->basePaletteBank - 1;
    } else {
        subject->paletteBank = subject->basePaletteBank;
        subject->countdown--;
        if (subject->countdown < subject->nextThreshold) {
            sub_082314C4(subject->effects);
            subject->nextThreshold -= 250;
        }
    }
}
