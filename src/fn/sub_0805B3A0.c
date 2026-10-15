#include "scene_panel_interaction.h"
#define ScenePanel_IsTetherDistant sub_0805B3A0
s32 ScenePanel_IsTetherDistant(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    if (sub_0805B28C(&gUnk_02000488->position, &panel->center, 800) != 0)
        return 0;
    return 1;
}
