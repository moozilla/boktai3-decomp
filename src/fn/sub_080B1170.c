// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"

#define gRandomIndex gUnk_03005308
#define gRandomTable gUnk_0203B400
struct SceneActorRandomReaction {
    u8 unknown0[0x690];
    u16 timer, period;
    u8 unknown694[0x6d4 - 0x694];
    u16 flags;
};
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
static inline u8 unavailable(struct SceneActorRegistryResetView *p)
{
    if (p) {
        u32 mask = 0x40;
        if (p->flags2D4 & mask)
            return 1;
        {
            u32 mask = 0x20000;
            if (p->flags2D0 & mask)
                return 1;
        }
        if (p->health <= 0)
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
static inline u32 blocked(struct SceneActorRegistryResetView *p)
{
    u32 mask = 0xe33f;
    return p->flags2D4 & mask;
}
void SceneActor_TickRandomReaction(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRandomReaction *context = p->context;
    if (!unavailable(p) && !blocked(p) && p->field2B5 == 0x23 && active(context)) {
        u16 remaining;
        register s32 timer asm("r4");
        remaining = context->timer;
        timer = remaining;
        if (timer) {
            remaining--;
            context->timer = remaining;
        } else {
            context->timer = context->period;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            {
                s32 random = *(u8 *)&gUnk_0203B400[gUnk_03005308];
                if (random > 0xbd) {
                    u32 behavior = 0x11;
                    p->state2A8 = 0;
                    p->phase2A9 = 0;
                    p->behavior2AA = behavior;
                    p->counter2AC = timer;
                    p->field2B1 = 1;
                }
            }
        }
    }
}
