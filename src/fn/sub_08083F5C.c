#include "scene_actor_control_helpers.h"
#include "scene_actor_terrain.h"
/* SceneActorControl_ReflectTerrainProbe: Reverses secondary horizontal motion and stops primary horizontal motion when projected terrain exceeds current ground height. */
#define SceneActorControl_ReflectTerrainProbe sub_08083F5C

void SceneActorControl_ReflectTerrainProbe(struct SceneActorRegistryResetView *actor)
{
    s16 *motionZ;
    struct ContactPosition target;
    {
        struct ContactPosition *p = &target;
        struct ContactPosition *position = &actor->body.position;
        struct ContactPosition *offset = &actor->secondaryMotion;
        s32 cell, height;
        p->x = offset->x + position->x;
        p->y = offset->y + position->y;
        p->z = offset->z + position->z;
        cell = SceneActorTerrain_GetCellIndex(&target);
        height = SceneActorTerrain_GetHeight(cell, &target);
        if (actor->groundHeight2BE < height) {
            s32 zero;
            s32 off = 0x2EC;
            s16 *x = (s16 *)((u8 *)actor + off);
            s32 value = -*x;
            zero = 0;
            *x = value;
            motionZ = &actor->secondaryMotion.z;
            value = -(*motionZ);
            *motionZ = value;
            actor->body.motion.x = zero;
            actor->body.motion.z = zero;
        }
    }
}
