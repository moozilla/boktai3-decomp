#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"
#define ActorTrail_ResetPositions     TentacleActor_ResetSegmentPositions
#define ActorTrail_CopyTerrainSamples TentacleActor_ResetRouteHistory
extern const u16 gUnk_0203B400[];
struct ActorTrail {
    u8 unknown0[0x875];
    u8 field875;
    u8 unknown876[3];
    u8 field879;
    u16 field87A;
    u8 unknown87C[0x54];
    u16 flags8D0;
    u8 unknown8D2[0x17];
    u8 variant;
    u8 unknown8EA[0xA];
    u16 field8F4;
    u8 unknown8F6[0x36];
    struct ContactPosition savedPosition;
};
void TentacleActor_ResetRouteHistory(void *);
void TentacleActor_ResetSegmentPositions(void *, struct ContactPosition *);
void TentacleActor_SelectWanderRoute(void *);
void ActorTrail_ResetAtSavedPosition(struct SceneActorRegistryResetView *actor)
{
    struct ActorTrail *trail = actor->context;
    struct ContactPosition *saved;
    trail->flags8D0 &= 0xFFFD;
    trail->field879 = 0;
    trail->field87A = 0;
    {
        u32 index = (gRandomIndex + 1) & 1023;
        u32 value, mask;
        gRandomIndex = index;
        index *= 2;
        value = *(const u16 *)((u32)gUnk_0203B400 + index);
        mask = 3;
        trail->variant = mask & value;
    }
    trail->field875 = 0;
    trail->field8F4 = 0;
    saved = &trail->savedPosition;
    if (!actor->drawableKind)
        *(struct ContactPosition *)((u8 *)actor->drawable + 0x1C) = *saved;
    else
        *(struct ContactPosition *)((u8 *)actor->drawable + 0x40) = *saved;
    actor->position = trail->savedPosition;
    TentacleActor_ResetRouteHistory(actor);
    TentacleActor_ResetSegmentPositions(trail, &actor->position);
    TentacleActor_SelectWanderRoute(actor);
    {
        u32 state = 2;
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = state;
        actor->counter2AC = 0;
    }
    actor->field2B1 = 1;
    {
        void *target = actor->entryTarget228;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = 1;
        actor->target = target;
    }
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
