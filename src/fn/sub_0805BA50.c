#include "scene_panel_interaction.h"
#define ScenePanel_WaitRigRelease sub_0805BA50
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
        sub_0805907C((void *)panel);
    if (anyActive)
        panel->frame = 0;
    else if (++panel->frame > 119) {
        sub_080595AC(&panel->rig, 6);
        sub_08057960((void *)panel, 11);
    }
}
