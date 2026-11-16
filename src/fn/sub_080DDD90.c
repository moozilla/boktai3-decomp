#include "global.h"
#include "dog.h"
#include "scene_actor_reset.h"

u8 Dog_IsActionTimerExpired(struct ChaseActor *);
void Dog_SelectRetreatOrChase(struct ChaseActor *);
void Dog_SelectTargetAction(struct ChaseActor *);
void SceneActorControl_OnIdle(void);
static inline u8 HasReaction(struct ChaseActor *p)
{
    struct SceneActorReactionMotion *motion = (void *)&p->base.tracking.target;
    u32 mask = 8;
    if (mask & motion->flags)
        return 1;
    return 0;
}
static inline u8 ContextBlocked(struct Dog *p)
{
    u32 mask = 8;
    if (p->flags & mask)
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
    v |= p->base.motionFlags;
    p->base.motionFlags = v;
}
void Dog_UpdateTargetGate(struct ChaseActor *p)
{
    struct Dog *context = p->base.context;
    u8 active;
    u32 v;
    u16 *f;
    context->timer690 = context->limitX;
    active = HasReaction(p);
    if (active) {
        Or16(&p->group.common.gate.flags, 4);
        v = 0x80;
        f = &p->base.motionFlags;
        v |= *f;
        *f = v;
        p->group.common.gate.counter = p->group.common.gate.duration;
        if (Test16(f, 0x100) || !Dog_IsActionTimerExpired(p)) {
            if (!ContextBlocked(context))
                Dog_SelectRetreatOrChase(p);
            return;
        } else {
            Dog_SelectTargetAction(p);
        }
    } else {
        u32 one;
        void (*callback)(void);
        u32 kind;
        And32(&context->flags, ~12);
        one = 1;
        p->base.state2A8 = active;
        p->base.phase2A9 = one;
        p->base.behavior2AA = active;
        p->base.counter2AC = active;
        p->base.field2B1 = one;
        callback = SceneActorControl_OnIdle;
        kind = 0x10;
        p->base.field2B2 = one;
        p->base.stepDone = active;
        p->base.controlKind = kind;
        p->base.target = callback;
        p->base.field2C8 = active;
        And32(&p->base.pendingControlFlags, ~1);
        p->base.pendingControl = active;
    }
    OrMotion((struct SceneActorReactionMotion *)&p->base.tracking.target, 8);
}
