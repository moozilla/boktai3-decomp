#include "objects/pile_driver/scene_panel_rig_setup.h"
void PileDriverRig_InitSprites(struct ScenePanelRig *rig)
{
    PileDriverRig_InitMainSprite(rig);
    PileDriverRig_InitPrimarySprites(rig);
    PileDriverRig_InitSecondarySprites(rig);
}
