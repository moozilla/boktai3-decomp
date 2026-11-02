#include "scene_actor_control_helpers.h"
#include "scene_actor_terrain_reaction.h"
void SceneActorControl_StartStoredCurve(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition position;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    position = actor->previousPosition;
    SceneActorControl_StartPositionCurve(actor, &position);
    SceneActorTerrain_EnterState(actor, 2, 0, 3);
    {
        u32 *flags = &SceneActorControl_GetMotionState(actor)->flags3D4;
        u32 mask = ~8;
        *flags &= mask;
    }
    actor->field2B1 = 0;
}