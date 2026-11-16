#include "global.h"
#include "scene_actor_reset.h"

// foundry draft (worker best 93.0%); what differs is mostly register allocation and instruction order
struct SceneActorReactionContext {
    u8 unknown0[0x6D0];
    u8 active;
    u8 unknown6D1[0xB];
    u32 flags6DC;
};
u8 SceneActorReaction_IsRangeReady(struct ChaseActor *);
void Skeleton_SelectDistanceReaction(struct ChaseActor *);
void SceneActorControl_SelectBehaviorByTargetAngle(struct ChaseActor *);
void SceneActorControl_OnIdle(void);
static inline u8 HasReaction(struct ChaseActor *p)
{
    struct SceneActorReactionMotion *motion = (void *)&p->base.tracking.target;
    u32 mask = 8;
    if (mask & motion->flags)
        return 1;
    return 0;
}
static inline u8 ContextBlocked(struct SceneActorReactionContext *p)
{
    u32 mask = 0x200;
    if (p->flags6DC & mask)
        return 1;
    return 0;
}
static inline void Or16(u16 *p, u32 mask)
{
    u32 value = mask;
    value |= *p;
    *p = value;
}
static inline u32 Test16(u16 *p, u32 mask)
{
    return *p & mask;
}
static inline void And32(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void OrMotion(struct SceneActorReactionMotion *p, u32 mask)
{
    u32 v = mask;
    v |= p->flags;
    p->flags = v;
}
static inline void SetFlags(struct ChaseActor *p, u32 mask)
{
    u32 v = mask;
    v |= p->base.flags2D4;
    p->base.flags2D4 = v;
}
void SceneActorReaction_UpdateGate(struct ChaseActor *p)
{
    struct SceneActorReactionContext *context = p->base.context;
    u8 active = HasReaction(p);
    u32 v;
    u16 *f;
    if (active) {
        Or16(&p->group.common.gate.flags, 4);
        v = 0x80;
        f = &p->base.flags2D4;
        v |= *f;
        *f = v;
        p->group.common.gate.counter = p->group.common.gate.duration;
        if (Test16(f, 0x100) || !SceneActorReaction_IsRangeReady(p) || context->active) {
            if (!ContextBlocked(context))
                Skeleton_SelectDistanceReaction(p);
        } else {
            SceneActorControl_SelectBehaviorByTargetAngle(p);
        }
    } else {
        u32 one = 1;
        void (*callback)(void);
        u32 kind;
        p->base.state2A8 = active;
        p->base.phase2A9 = one;
        p->base.behavior2AA = active;
        p->base.counter2AC = active;
        p->base.field2B1 = one;
        callback = SceneActorControl_OnIdle;
        kind = 0x10;
        p->base.field2B2 = one;
        p->base.field2B0 = active;
        p->base.field2B5 = kind;
        p->base.target = callback;
        p->base.field2C8 = active;
        And32(&p->base.flags114, ~1);
        p->base.word104 = active;
        OrMotion((struct SceneActorReactionMotion *)&p->base.tracking.target, 8);
    }
}
