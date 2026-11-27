#include "objects/pile_driver/pile_driver_rig_setup.h"
void PileDriverRig_InitSprites(struct PileDriverRig *rig)
{
    PileDriverRig_InitMainSprite(rig);
    PileDriverRig_InitPrimarySprites(rig);
    PileDriverRig_InitSecondarySprites(rig);
}
