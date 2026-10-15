#include "scene_panel_rig_setup.h"
struct SceneRigColliderVector {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
void ScenePanelRig_InitCollider(struct ScenePanelRig *rig, u32 power)
{
    struct SceneRigColliderVector size, offset;
    void *collider = &rig->collider;
    size.x = 128;
    size.y = 128;
    size.z = 128;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x4001, 0, 0x10, &size, &offset);
    sub_0821FF7C(collider, power, 0, 0);
    Collider_SetPosition(collider, &rig->position, 0);
    Collider_SetCallback(collider, ScenePanelRig_ApplyContact, rig);
    Collider_Refresh(collider);
}
