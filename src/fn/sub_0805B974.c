#include "scene_panel_interaction.h"
#define ScenePanel_AdvanceArrivalRamp sub_0805B974
void ScenePanel_AdvanceArrivalRamp(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    u16 *frame = &panel->frame;
    if (*frame <= 69) {
        *frame += 1;
        if (*frame == 70)
            sub_08057978(input, 3);
    }
    if (panel->arrivalRampEnabled) {
        u16 *fade = &panel->arrivalRampFrame;
        if (*fade <= 59)
            *fade += 1;
        sub_082260A4(*fade >> 1);
    }
}
