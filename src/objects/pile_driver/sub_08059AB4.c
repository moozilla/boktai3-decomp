#include "objects/pile_driver/pile_driver_rig.h"
/* PileDriverRig_TickHidden: Holds the collider timer at 16 during the rig's hidden state */
void PileDriverRig_TickHidden(struct PileDriverRig *rig)
{
    rig->colliderHold = 16;
}
