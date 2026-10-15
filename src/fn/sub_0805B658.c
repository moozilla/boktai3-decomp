#include "scene_panel_interaction.h"
void ScenePanel_WaitSettledSubjects(struct ScenePanel *input)
{
    if (ScenePanel_AreSubjectsSettled(input) != 0) {
        sub_0815C99C(SCENE_PANEL_PLAYER);
        ScenePanel_SetEffectState(input, 3);
        ScenePanel_CallPendingScript(input, 0);
    }
}
