#include "scene_panel_rig.h"
#define ScenePanelRig_TickHideBurst sub_0805A40C
void ScenePanelRig_TickHideBurst(struct ScenePanelRig *rig)
{
    struct ContactPackedVector extent;
    u16 *counter;
    rig->scale = 16;
    counter = &rig->stateFrame;
    if (*counter <= 63) {
        if (*counter == 0) {
            ScenePanelRig_HideSprites(rig);
            ((struct SceneRigParentView *)gUnk_020004A4)->activated = 1;
        }
        {
            u32 frame = *counter;
            if (frame <= 31 || (frame & 1) == 0) {
                extent.x = (64 - frame) * 2;
                extent.y = extent.x;
                extent.z = extent.x;
                sub_0806A160(&rig->position, 1, &extent, 0, 0, 0, 2, 2);
                if (*counter <= 31) {
                    ScenePanel_SetPresentationMode(rig->parent);
                    sub_0806A160(&rig->secondary[Mod(*counter, 5)].position, 1, &extent, 0, 0, 0, 2,
                                 2);
                }
            }
        }
        rig->stateFrame++;
    } else
        ScenePanelRig_SetState(rig, 0);
}
