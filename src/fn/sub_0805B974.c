#include "scene_panel_interaction.h"
#define ScenePanel_AdvanceArrivalFade sub_0805B974
void ScenePanel_AdvanceArrivalFade(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    u16 *frame = &panel->frame;
    if (*frame <= 69) {
        *frame += 1;
        if (*frame == 70)
            sub_08057978(input, 3);
    }
    if (panel->fadeEnabled) {
        u16 *fade = &panel->fadeFrame;
        if (*fade <= 59)
            *fade += 1;
        sub_082260A4(*fade >> 1);
    }
}
