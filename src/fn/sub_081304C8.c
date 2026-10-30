// MATCHING_CONCESSION: register-pinning
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
static inline u8 TakeReset(struct SceneActorRegistryResetView *a)
{
    if (a->field2B1) {
        a->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void BeginControlZero(struct SceneActorRegistryResetView *actor, void *control,
                                    u32 kind)
{
    u32 offset;
    u32 zero;
    {
        u8 *field = &actor->field2B2;
        zero = 0;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state)
{
    u8 *field = &actor->state2A8;
    u32 zero = 0;
    *field = state;
    actor->phase2A9 = zero;
    actor->behavior2AA = zero;
    {
        u32 *counter = &actor->counter2AC;
        *counter = zero;
    }
    actor->field2B1 = 1;
}
static inline void SetFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 value = mask;
    asm("" : "+r"(value));
    value |= actor->flags2D4;
    actor->flags2D4 = value;
}
void SceneActorControl_HideOffscreen(struct SceneActorRegistryResetView *actor)
{
    if (TakeReset(actor)) {
        struct ContactPosition *position;
        if (actor->drawableKind == 0)
            position = (void *)((u8 *)actor->drawable + 0x1C);
        else
            position = (void *)((u8 *)actor->drawable + 0x40);
        position->y = 30000;
    }
    actor->body.position.x = 0;
    actor->body.position.y = 30000;
    actor->body.position.z = 0;
    {
        u32 mask = 1;
        if (actor->drawableKind == 0)
            FIELD(actor->drawable, 0, u32) |= mask;
        else {
            struct DrawablePart *part = (void *)((u8 *)actor->drawable + 0x20);
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
    actor->counter2AC++;
}
