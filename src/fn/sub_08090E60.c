#include "global.h"
#include "scene_actor_motion.h"
#include "scene_actor_terrain.h"
#define SceneActorMotion_RefreshGroundHeight sub_08090E60

void sub_08090AE0(struct SceneActorMotionView *);

s32 sub_08090E60(u8 *argument)
{
    struct SceneActorMotionView *actor = (void *)argument;

    if ((s32)*(u16 *)(argument + 0x2BE) - *(u16 *)(argument + 0x3D8) > 0x1FF) {
        s32 cell = SceneActorTerrain_GetCellIndex((struct ContactPosition *)(argument + 0x5C));
        if (sub_0821D64C(cell, 1)) {
            sub_08090AE0(actor);
            actor->groundHeight = *(u16 *)(argument + 0x3D8);
        }
    }
    {
        u16 value = actor->groundHeight;
        *(u16 *)(argument + 0x3D8) = value;
    }
    return 1;
}
