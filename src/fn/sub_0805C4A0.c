#include "scene_panel_orchestration.h"
void ScenePanel_WriteFullScriptStatus(void)
{
    if (gUnk_020004A4) {
        struct ScriptUtilityReference reference;
        s32 i;
        sub_0821AAD8(&reference);
        sub_0821B4C8(&reference, 0, SCENE_PANEL_GLOBAL->initialMode);
        sub_0821AAD8(&reference);
        if (gUnk_02000488) {
            if (gUnk_02000488->state == 12)
                sub_0821B4C8(&reference, 0, 1);
            else
                sub_0821B4C8(&reference, 0, 0);
        } else
            sub_0821B4C8(&reference, 0, 0);
        i = 0;
        do {
            struct ScenePanelInteractionSubject *subject;
            sub_0821AAD8(&reference);
            subject = SCENE_PANEL_GLOBAL->subjects[i];
            if (!subject || subject->state <= 1)
                sub_0821B4C8(&reference, 0, 0);
            else if (subject->state == 4)
                sub_0821B4C8(&reference, 0, 4);
            else
                sub_0821B4C8(&reference, 0, 2);
            i++;
        } while (i <= 3);
    }
}
