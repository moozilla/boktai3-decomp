#include "scene_panel_interaction.h"
void ScenePanel_WaitTransitionGate(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    u16 *frame = &panel->frame;
    if (*frame <= 29)
        *frame += 1;
    else if (!panel->gateC12) {
        Sound_PlaySong(490);
        gUnk_02000488->state = 14;
        ScenePanel_SetEffectState(input, 9);
    }
}
