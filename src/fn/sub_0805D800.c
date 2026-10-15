#include "scene_panel_subject_tail.h"
s32 ScenePanelSubject_Update(struct ScenePanelSubjectView *subject)
{
    if (subject->state)
        ScenePanelSubject_KeepPlayerOutside((void *)subject);
    if (subject->timer)
        subject->timer--;
    if (subject->contactCooldown)
        subject->contactCooldown--;
    subject->update(subject);
    ScenePanelSubject_UpdatePaletteBank(subject);
    {
        u16 *hold = &subject->hold;
        if (*hold) {
            u32 mode = SCENE_PANEL_GLOBAL->mode;
            if (mode) {
                if (subject->state != 4)
                    *hold = 0;
                else {
                    subject->collider.cost = 0;
                    (*hold)--;
                }
            } else
                *hold = mode;
        }
    }
    return 0;
}
