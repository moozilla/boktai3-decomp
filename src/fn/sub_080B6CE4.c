#include "global.h"
#include "scene_actor_reset.h"

#ifdef NONMATCHING
// fuzzy: 93.6%
// foundry draft (worker best 93.0%); what differs is mostly register allocation and instruction order
#define SceneActorReaction_UpdateGate sub_080B6CE4
struct SceneActorReactionContext {
    u8 unknown0[0x6D0];
    u8 active;
    u8 unknown6D1[0xB];
    u32 flags6DC;
};
u8 SceneActorReaction_IsRangeReady(struct SceneActorRegistryResetView *);
void sub_080B7C10(struct SceneActorRegistryResetView *);
void sub_080B6BA8(struct SceneActorRegistryResetView *);
void SceneActorControl_OnIdle(void);
static inline u8 HasReaction(struct SceneActorRegistryResetView *p)
{
    struct SceneActorReactionMotion *motion = (void *)&p->fieldDC;
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
    p->flags = mask | p->flags;
}
static inline void SetFlags(struct SceneActorRegistryResetView *p, u32 mask)
{
    u32 v = mask;
    v |= p->flags2D4;
    p->flags2D4 = v;
}
void sub_080B6CE4(struct SceneActorRegistryResetView *p)
{
    struct SceneActorReactionContext *context = p->context;
    u8 active = HasReaction(p);
    if (active) {
        Or16(&p->group.flags414, 4);
        Or16(&p->flags2D4, 0x80);
        p->group.field418 = p->group.field416;
        if (Test16(&p->flags2D4, 0x100) || !SceneActorReaction_IsRangeReady(p) || context->active) {
            if (!ContextBlocked(context))
                sub_080B7C10(p);
        } else {
            sub_080B6BA8(p);
        }
    } else {
        u32 one = 1;
        void (*callback)(void);
        u32 kind;
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
        OrMotion((struct SceneActorReactionMotion *)&p->fieldDC, 8);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B6CE4);
#endif
