#include "scene_panel_interaction.h"
#define ScenePanel_WaitTetherOrActiveSubject sub_0805B688
void ScenePanel_WaitTetherOrActiveSubject(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    s32 i, count;
    if (gUnk_02000488->state != 12 && sub_0805B2CC((void *)panel)) {
        sub_0815B15C(SCENE_PANEL_PLAYER, 0, -1, 0, 0);
        panel->variant = 1;
        ScenePanel_SetEffectState((void *)panel, 1);
    }
    i = 0;
    count = panel->subjectCount;
    for (; i < count; i++) {
        if (panel->subjects[i]->state == 4) {
            ScenePanel_SetEffectState((void *)panel, 4);
            break;
        }
    }
}
