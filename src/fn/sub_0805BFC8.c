#include "scene_panel_config.h"
void ScenePanel_InitAnchorPositions(struct ScenePanelConfigView *parent)
{
    if (Script_SeekToKeyword('c')) {
        parent->origin.x = Script_GetValue();
        parent->origin.y = Script_GetValue();
        parent->origin.z = Script_GetValue();
    }
    parent->approachPosition = parent->origin;
    parent->approachPosition.x += 324;
    parent->approachPosition.z += 324;
    parent->center = parent->origin;
    parent->center.x -= 20;
    parent->center.z -= 20;
}
