#include "scene_panel_orchestration.h"
void ScenePanel_WriteScriptStatus(void)
{
    if (gUnk_020004A4) {
        struct ScriptUtilityReference reference;
        s32 i;
        Script_ReadReference(&reference);
        if (gUnk_02000488) {
            if (gUnk_02000488->state == 12)
                Script_WriteReferenceValue(&reference, 0, 1);
            else
                Script_WriteReferenceValue(&reference, 0, 0);
        } else
            Script_WriteReferenceValue(&reference, 0, 0);
        i = 0;
        do {
            struct ScenePanelInteractionSubject *subject;
            Script_ReadReference(&reference);
            subject = SCENE_PANEL_GLOBAL->subjects[i];
            if (!subject || subject->state <= 1)
                Script_WriteReferenceValue(&reference, 0, 0);
            else if (subject->state == 4)
                Script_WriteReferenceValue(&reference, 0, 4);
            else
                Script_WriteReferenceValue(&reference, 0, 2);
            i++;
        } while (i <= 3);
    }
}
