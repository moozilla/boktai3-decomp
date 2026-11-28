#include "monsters/centipede/centipede_body.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
void Centipede_ResetRouteHistory(void *);
void Centipede_ResetSegmentPositions(void *, struct ContactPosition *);
void Centipede_SelectWanderRoute(void *);
void ActorTrail_ResetAtSavedPosition(struct ChaseActor *actor)
{
    struct TentacleBody *trail = actor->base.context;
    struct ContactPosition *saved;
    trail->attackFlags &= ~2;
    trail->blockedFrameCount = 0;
    trail->returnCell = 0;
    {
        u32 index = (gRandomIndex + 1) & 1023;
        u32 value, mask;
        gRandomIndex = index;
        index *= 2;
        value = *(const u16 *)((u32)gRandomTable + index);
        mask = 3;
        trail->wanderDirection = mask & value;
    }
    trail->spin = 0;
    trail->unknown8F4 = 0;
    saved = &trail->savedPosition;
    if (!actor->base.drawableKind)
        *(struct ContactPosition *)((u8 *)actor->base.drawable + 0x1C) = *saved;
    else
        *(struct ContactPosition *)((u8 *)actor->base.drawable + 0x40) = *saved;
    actor->base.body.position = trail->savedPosition;
    Centipede_ResetRouteHistory(actor);
    Centipede_ResetSegmentPositions(trail, &actor->base.body.position);
    Centipede_SelectWanderRoute(actor);
    SceneActorTerrain_EnterState(actor, 0, 0, 2);
    SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
}
