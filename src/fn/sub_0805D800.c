#include "scene_panel_subject_tail.h"
#define ScenePanelSubject_Update sub_0805D800
s32 ScenePanelSubject_Update(struct ScenePanelSubjectTail *subject)
{
    if (subject->state)
        sub_0805CC44((void *)subject);
    if (subject->timer)
        subject->timer--;
    if (subject->contactCooldown)
        subject->contactCooldown--;
    subject->update(subject);
    sub_0805D770(subject);
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
