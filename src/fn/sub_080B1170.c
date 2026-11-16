#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"
#include "ram.h"

struct SceneActorRandomReaction {
    u8 unknown0[0x690];
    u16 timer, period;
    u8 unknown694[0x6d4 - 0x694];
    u16 flags;
};
static inline u8 unavailable(struct ChaseActor *p)
{
    if (p) {
        u32 mask = 0x40;
        if (p->base.motionFlags & mask)
            return 1;
        {
            u32 mask = 0x20000;
            if (p->base.statusFlags & mask)
                return 1;
        }
        if (p->base.health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 active(struct SceneActorRandomReaction *p)
{
    u32 mask = 1;
    if (p->flags & mask)
        return 1;
    return 0;
}
static inline u32 blocked(struct ChaseActor *p)
{
    u32 mask = 0xe33f;
    return p->base.motionFlags & mask;
}
void SceneActor_TickRandomReaction(struct ChaseActor *p)
{
    struct SceneActorRandomReaction *context = p->base.context;
    if (!unavailable(p) && !blocked(p) && p->base.controlKind == 0x23 && active(context)) {
        if (context->timer) {
            context->timer--;
        } else {
            context->timer = context->period;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            {
                s32 random = *(u8 *)&gRandomTable[gRandomIndex];
                if (random > 0xbd) {
                    u32 behavior = 0x11;
                    p->base.state2A8 = 0;
                    p->base.phase2A9 = 0;
                    p->base.behavior2AA = behavior;
                    p->base.counter2AC = 0;
                    p->base.field2B1 = 1;
                }
            }
        }
    }
}
