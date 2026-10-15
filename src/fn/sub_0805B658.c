#include "scene_panel_interaction.h"
#define ScenePanel_WaitSettledSubjects sub_0805B658
void ScenePanel_WaitSettledSubjects(struct ScenePanel *input)
{
    if (sub_0805B318(input) != 0) {
        sub_0815C99C(SCENE_PANEL_PLAYER);
        ScenePanel_SetEffectState(input, 3);
        ScenePanel_CallPendingScript(input, 0);
    }
}
