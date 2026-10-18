#include "scene_panel_rig.h"
/* ScenePanelRig_TickHidden: Holds the collider timer at 16 during the rig's hidden state */
#define ScenePanelRig_TickHidden sub_08059AB4
#define ScenePanelRig_TickHidden sub_08059AB4
void ScenePanelRig_TickHidden(struct ScenePanelRig *rig)
{
    rig->colliderHold = 16;
}
