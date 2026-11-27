#include "objects/pile_driver/pile_driver_rig_setup.h"
void PileDriverRig_InitPayloadVectors(struct PileDriverRig *rig)
{
    struct SceneRigPayloadView *view = (void *)rig;
    view->firstX = 160;
    view->firstY = 160;
    view->firstZ = 160;
    view->secondX = 64;
    view->secondY = 32;
    view->secondZ = 64;
}
