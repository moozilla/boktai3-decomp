#define SceneActorMotion_AdvanceIdlePhase sub_0813042C
#include "global.h"
#include "scene_actor_motion.h"
static inline u8 TakeReset(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u16 MaskFlags(struct SceneActorMotionView *p, u32 mask)
{
    return mask & p->flags2D4;
}
void sub_0813042C(struct SceneActorMotionView *actor)
{
    u16 flags;
    TakeReset(&actor->resetRequested);
    flags = MaskFlags(actor, 0x20);
    if (!flags) {
        u32 behavior = 10;
        u32 offset = 0x2A8;
        ((u8 *)actor)[offset] = flags;
        ++offset;
        ((u8 *)actor)[offset] = flags;
        ++offset;
        ((u8 *)actor)[offset] = behavior;
        actor->counter = 0;
        *(u8 *)((u32)actor + 0x2B1) = 1;
    }
    ++actor->counter;
}
