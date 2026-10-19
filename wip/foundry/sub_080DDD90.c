#define SceneActorControl_UpdateGateTransition  sub_080DDD90
#define SceneActorControl_IsContextTimerExpired sub_080DF298
#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_behavior.h"
struct SceneActorGateContext {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x14];
    u16 savedValue, value692;
};
u8 sub_080DF298(struct SceneActorRegistryActor *);
void sub_080DF400(struct SceneActorRegistryActor *);
void sub_080DE128(struct SceneActorRegistryActor *);
void sub_0807FC90(void);
static inline u8 TestGate(struct SceneActorBehaviorGateView *gate)
{
    u32 mask = 8;
    if (gate->flags & mask)
        return 1;
    return 0;
}
static inline u8 TestContext(struct SceneActorGateContext *context)
{
    u32 mask = 8;
    if (context->flags & mask)
        return 1;
    return 0;
}
static inline u32 TestMask(u16 *p, u32 mask)
{
    return *p & mask;
}
static inline void ResetGate(struct SceneActorRegistryActor *actor, s32 active,
                             struct SceneActorGateContext *context)
{
    u32 offset, odd, third;
    void (*callback)(void);
    u8 kind;
    u32 *flags = &context->flags;
    s32 mask = -13;
    u32 one;
    *flags &= mask;
    one = 1;
    offset = 0x2A8;
    ((u8 *)actor)[offset] = active;
    odd = 0x2A9;
    ((u8 *)actor)[odd] = one;
    third = 0x2AA;
    ((u8 *)actor)[third] = active;
    offset += 4;
    *(u32 *)((u8 *)actor + offset) = active;
    odd += 8;
    ((u8 *)actor)[odd] = one;
    callback = sub_0807FC90;
    kind = 16;
    offset += 6;
    ((u8 *)actor)[offset] = one;
    {
        u32 field = 0x2B0;
        ((u8 *)actor)[field] = active;
        offset += 3;
        ((u8 *)actor)[offset] = kind;
        field -= 0x94;
        *(void (**)(void))((u8 *)actor + field) = callback;
    }

    actor->field2C8 = active;
    {
        u32 *f = &actor->flags114;
        s32 m = -2;
        *f &= m;
    }
    actor->field104 = active;
}
void sub_080DDD90(struct SceneActorRegistryActor *actor)
{
    s32 active;
    struct SceneActorGateContext *context = actor->context;
    context->savedValue = context->value692;
    active = TestGate(&((struct SceneActorBehaviorView *)actor)->gate);
    if (active) {
        {
            u16 *f = &actor->flags414;
            u32 mask = 4;
            mask |= *f;
            *f = mask;
        }
        {
            u32 mask = 0x80;
            mask |= actor->flags2D4;
            actor->flags2D4 = mask;
        }
        {
            u32 off = 0x416;
            u16 value = *(u16 *)((u8 *)actor + off);
            off += 2;
            *(u16 *)((u8 *)actor + off) = value;
        }
        if ((TestMask(&actor->flags2D4, 0x100)) || !sub_080DF298(actor)) {
            if (!TestContext(context))
                sub_080DF400(actor);
            return;
        }
        sub_080DE128(actor);
    } else {
        ResetGate(actor, active, context);
    }
    {
        struct SceneActorBehaviorGateView *gate = &((struct SceneActorBehaviorView *)actor)->gate;
        u32 mask = 8;
        mask |= gate->flags;
        gate->flags = mask;
    }
}
