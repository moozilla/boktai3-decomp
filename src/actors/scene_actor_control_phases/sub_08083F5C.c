#include "engine/shared/scene_actor_control_helpers.h"
#include "engine/shared/scene_actor_terrain.h"
/* SceneActorControl_ReflectTerrainProbe: Reverses secondary horizontal motion and stops primary horizontal motion when projected terrain exceeds current ground height. */
#define SceneActorControl_ReflectTerrainProbe sub_08083F5C

void SceneActorControl_ReflectTerrainProbe(struct ChaseActor *actor)
{
    s16 *motionZ;
    struct ContactPosition target;
    {
        struct ContactPosition *p = &target;
        struct ContactPosition *position = &actor->base.body.position;
        struct ContactPosition *offset = &actor->base.secondaryMotion;
        s32 cell, height;
        p->x = offset->x + position->x;
        p->y = offset->y + position->y;
        p->z = offset->z + position->z;
        cell = SceneActorTerrain_GetCellIndex(&target);
        height = SceneActorTerrain_GetHeight(cell, &target);
        if (actor->base.groundHeight < height) {
            s32 zero;
            s16 *x = &actor->base.secondaryMotion;
            s32 value = -*x;
            zero = 0;
            *x = value;
            motionZ = &actor->base.secondaryMotion.z;
            value = -(*motionZ);
            *motionZ = value;
            actor->base.body.motion.x = zero;
            actor->base.body.motion.z = zero;
        }
    }
}
