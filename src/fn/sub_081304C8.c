#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
struct DrawablePart {
    u32 unknown0[2];
    u32 flags;
};
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
// confidence=low: hides the drawable and moves the actor above the active field.
#define SceneActorControl_HideOffscreen sub_081304C8
static inline u8 TakeReset(struct ChaseActor *a)
{
    if (a->base.field2B1) {
        a->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void BeginControlZero(struct ChaseActor *actor, void *control, u32 kind)
{
    u32 offset;
    u32 zero;
    {
        u8 *field = &actor->base.field2B2;
        zero = 0;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->base.field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = zero;
}
static inline void SetState(struct ChaseActor *actor, u32 state)
{
    u8 *field = &actor->base.state2A8;
    u32 zero = 0;
    *field = state;
    actor->base.phase2A9 = zero;
    actor->base.behavior2AA = zero;
    {
        u32 *counter = &actor->base.counter2AC;
        *counter = zero;
    }
    actor->base.field2B1 = 1;
}
static inline void SetFlags(struct ChaseActor *actor, u32 mask)
{
    u16 value = mask;
    value |= actor->base.flags2D4;
    actor->base.flags2D4 = value;
}
void SceneActorControl_HideOffscreen(struct ChaseActor *actor)
{
    if (TakeReset(actor)) {
        struct ContactPosition *position;
        if (actor->base.drawableKind == 0)
            position = (void *)((u8 *)actor->base.drawable + 0x1C);
        else
            position = (void *)((u8 *)actor->base.drawable + 0x40);
        position->y = 30000;
    }
    actor->base.body.position.x = 0;
    actor->base.body.position.y = 30000;
    actor->base.body.position.z = 0;
    {
        u32 mask = 1;
        if (actor->base.drawableKind == 0)
            FIELD(actor->base.drawable, 0, u32) |= mask;
        else {
            struct DrawablePart *part = (void *)((u8 *)actor->base.drawable + 0x20);
            part->flags |= mask;
        }
    }
    SetState(actor, 3);
    BeginControlZero(actor, SceneActorControl_OnIdle, 16);
    SceneActorTerrain_DisableCollision(actor);
    {
        u32 mask = 0x40;
        SetFlags(actor, mask);
    }
    actor->base.counter2AC++;
}
