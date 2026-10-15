#include "scene_panel_rig_setup.h"
void ScenePanelRig_InitPayloadVectors(struct ScenePanelRig *rig)
{
    struct SceneRigPayloadView *view = (void *)rig;
    view->firstX = 160;
    view->firstY = 160;
    view->firstZ = 160;
    view->secondX = 64;
    view->secondY = 32;
    view->secondZ = 64;
}
