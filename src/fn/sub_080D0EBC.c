#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
struct SceneActorDirectionalSequence {
    u8 unknown0[8];
    u16 frame;
    u8 unknownA[4];
    u16 timer;
};
void Actor_InitLaunchVelocity(struct ChaseActor *);
s32 Math_Atan2(s32, s32);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline u32 ReadFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.statusFlags & mask;
}
void SceneActor_UpdateDirectionalAnimation(struct ChaseActor *actor)
{
    u8 *direction = (u8 *)actor + 0x50;
    struct SceneActorDirectionalSequence *sequence = (void *)((u8 *)actor->base.drawable + 0x48);
    if (TakeFlag(&actor->base.field2B2)) {
        u32 mask;
        Actor_InitLaunchVelocity(actor);
        direction[9] = Math_Atan2(actor->base.secondaryMotion.x, actor->base.secondaryMotion.z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        mask = 0x100;
        actor->base.motionFlags |= mask;
    }
    if (sequence->frame > 2) {
        if (sequence->frame == 4 && sequence->timer == 0)
            Sound_PlaySong(0x101);
    }
    if (ReadFlags(actor, 0x10)) {
        u16 *flags = &actor->base.motionFlags;
        u32 mask = ~0x100;
        mask &= *flags;
        *flags = mask;
        actor->base.stepDone = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    } else {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
}
