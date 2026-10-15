#include "scene_panel_interaction.h"
#define ScenePanel_WaitRigIdle sub_0805BAE4
void ScenePanel_WaitRigIdle(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    if (panel->rig.state == 0) {
        Sound_PlaySong(491);
        TetherActor_BeginDirectedState(&panel->center);
        ScenePanel_SetEffectState(input, 12);
        ScenePanel_CallPendingScript(input, 5);
    }
}
