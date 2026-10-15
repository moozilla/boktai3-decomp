#include "scene_panel_interaction.h"
s32 ScenePanel_IsTetherDistant(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    if (ScenePanel_IsWithinHorizontalRadius(&gUnk_02000488->position, &panel->center, 800) != 0)
        return 0;
    return 1;
}
