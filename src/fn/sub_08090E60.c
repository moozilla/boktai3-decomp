#include "global.h"
#include "scene_actor_motion.h"
#include "scene_actor_terrain.h"

void SceneActorMotion_ArmPrimaryCollider(struct SceneActorMotionView *);

s32 SceneActorMotion_RefreshGroundHeight(u8 *argument)
{
    struct SceneActorMotionView *actor = (void *)argument;

    if ((s32)*(u16 *)(argument + 0x2BE) - *(u16 *)(argument + 0x3D8) > 0x1FF) {
        s32 cell = SceneActorTerrain_GetCellIndex((struct ContactPosition *)(argument + 0x5C));
        if (TerrainModifiers_FindCellOverride(cell, 1)) {
            SceneActorMotion_ArmPrimaryCollider(actor);
            actor->groundHeight = *(u16 *)(argument + 0x3D8);
        }
    }
    {
        u16 value = actor->groundHeight;
        *(u16 *)(argument + 0x3D8) = value;
    }
    return 1;
}
