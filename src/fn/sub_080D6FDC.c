#include "global.h"

#include "scene_actor_reaction.h"
#include "scene_actor_straight_route.h"

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline void SetControl(struct SceneActorRegistryResetView *actor)
{
    void *callback = SceneActorControl_OnIdle;
    u8 kind = 0;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

// confidence=low: scans to the last open cell in the current facing and starts a route.
u8 Mummy_RequestStraightRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    u8 *nav = (u8 *)actor + 0x3D4;
    s32 dest;
    s32 dir;
    struct ContactPosition pos;
    u32 result;

    if (FIELD(state, 0x6D2, u16))
        return 0;
    dir = ((actor->body.direction + 0x20) >> 6) & 3;
    dest = SceneActorTerrain_LastOpenCell(actor, dir);
    if (dest == actor->terrainCell)
        return 0;
    SceneActorTerrain_GetCenter(dest, &pos);
    pos.y = actor->body.position.y;
    result = SceneActorTerrain_RequestPath(actor, FIELD(nav, 0x1FC, void *), &actor->body.position, &pos);
    if (!result) {
        SetControl(actor);
        SetAnimation(actor, 3, 0, 0);
        return 0;
    }
    FIELD(state, 0x6D2, u16) = 0xF0;
    return 1;
}
