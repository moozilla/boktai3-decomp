#include "scene_panel_rig_setup.h"
void ScenePanelRig_InitSprites(struct ScenePanelRig *rig)
{
    ScenePanelRig_InitMainSprite(rig);
    ScenePanelRig_InitPrimarySprites(rig);
    ScenePanelRig_InitSecondarySprites(rig);
}
