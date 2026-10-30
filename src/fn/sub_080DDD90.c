#include "global.h"
#include "scene_actor_reset.h"

struct DogGateContext {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x14];
    u16 timer690;
    u16 timer692;
};
u8 Dog_IsActionTimerExpired(struct SceneActorRegistryResetView *);
void Dog_SelectRetreatOrChase(struct SceneActorRegistryResetView *);
void Dog_SelectTargetAction(struct SceneActorRegistryResetView *);
void SceneActorControl_OnIdle(void);
static inline u8 HasReaction(struct SceneActorRegistryResetView *p)
{
    struct SceneActorReactionMotion *motion = (void *)&p->tracking.target;
    u32 mask = 8;
    if (mask & motion->flags)
        return 1;
    return 0;
}
static inline u8 ContextBlocked(struct DogGateContext *p)
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
static inline void SetFlags(struct SceneActorRegistryResetView *p, u32 mask)
{
    u32 v = mask;
    v |= p->flags2D4;
    p->flags2D4 = v;
}
void Dog_UpdateTargetGate(struct SceneActorRegistryResetView *p)
{
    struct DogGateContext *context = p->context;
    u8 active;
    u32 v;
    u16 *f;
    context->timer690 = context->timer692;
    active = HasReaction(p);
    if (active) {
        Or16(&p->group.flags414, 4);
        v = 0x80;
        f = &p->flags2D4;
        v |= *f;
        *f = v;
        p->group.field418 = p->group.field416;
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
        p->state2A8 = active;
        p->phase2A9 = one;
        p->behavior2AA = active;
        p->counter2AC = active;
        p->field2B1 = one;
        callback = SceneActorControl_OnIdle;
        kind = 0x10;
        p->field2B2 = one;
        p->field2B0 = active;
        p->field2B5 = kind;
        p->target = callback;
        p->field2C8 = active;
        And32(&p->flags114, ~1);
        p->word104 = active;
    }
    OrMotion((struct SceneActorReactionMotion *)&p->tracking.target, 8);
}
