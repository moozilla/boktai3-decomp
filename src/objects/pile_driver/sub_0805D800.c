#include "objects/pile_driver/scene_panel_subject_tail.h"
s32 PileDriverSubject_Update(struct ScenePanelSubject *subject)
{
    if (subject->state)
        PileDriverSubject_KeepPlayerOutside((void *)subject);
    if (subject->timer)
        subject->timer--;
    if (subject->contactCooldown)
        subject->contactCooldown--;
    subject->update(subject);
    PileDriverSubject_UpdatePaletteBank(subject);
    {
        u16 *hold = &subject->hold;
        if (*hold) {
            u32 mode = SCENE_PANEL_GLOBAL->paletteMode;
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
