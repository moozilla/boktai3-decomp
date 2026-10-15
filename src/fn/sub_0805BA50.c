#include "scene_panel_interaction.h"
void ScenePanel_WaitRigRelease(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    s32 anyActive = 0;
    if (panel->mode) {
        u32 count = panel->subjectCount;
        if (anyActive < (s32)count) {
            struct ScenePanelInteractionSubject **subject = panel->subjects;
            s32 remaining = count;
            do {
                if ((*subject)->state == 4)
                    anyActive = 1;
                subject++;
                remaining--;
            } while (remaining != 0);
        }
    }
    if (gUnk_02000710->unk618 > 0)
        ScenePanel_RunScheduledAction((void *)panel);
    if (anyActive)
        panel->frame = 0;
    else if (++panel->frame > 119) {
        ScenePanelRig_SetState(&panel->rig, 6);
        ScenePanel_SetEffectState((void *)panel, 11);
    }
}
