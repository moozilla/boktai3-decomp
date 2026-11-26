#include "monsters/tentacle/tentacle_body.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "engine/engine.h"
#include "engine/ram.h"
#define ActorTrail_ResetPositions     TentacleActor_ResetSegmentPositions
#define ActorTrail_CopyTerrainSamples TentacleActor_ResetRouteHistory
void TentacleActor_ResetRouteHistory(void *);
void TentacleActor_ResetSegmentPositions(void *, struct ContactPosition *);
void TentacleActor_SelectWanderRoute(void *);
void ActorTrail_ResetAtSavedPosition(struct ChaseActor *actor)
{
    struct TentacleBody *trail = actor->base.context;
    struct ContactPosition *saved;
    trail->attackFlags &= 0xFFFD;
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
    TentacleActor_ResetRouteHistory(actor);
    TentacleActor_ResetSegmentPositions(trail, &actor->base.body.position);
    TentacleActor_SelectWanderRoute(actor);
    {
        u32 state = 2;
        actor->base.state2A8 = 0;
        actor->base.phase2A9 = 0;
        actor->base.behavior2AA = state;
        actor->base.counter2AC = 0;
    }
    actor->base.field2B1 = 1;
    SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
}
