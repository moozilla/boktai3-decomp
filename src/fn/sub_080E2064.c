#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
struct SceneActorTargetMotionContext {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x10];
    u32 timeLimit;
    u8 unknown690[0x17];
    u8 phase;
};
s32 sub_080DEF78(struct ChaseActor *, u32);
static inline u8 TakePrepared(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline u8 IsActive(u32 flags)
{
    if (flags)
        return 1;
    return 0;
}
static inline void SetPhase(struct ChaseActor *a, u32 phase)
{
    u8 *state = &a->base.state2A8;
    u32 zero = 0;
    *state = zero;
    a->base.phase2A9 = zero;
    a->base.behavior2AA = phase;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}
void SceneActorMotion_UpdateTargetTransition(struct ChaseActor *a)
{
    u32 *flags;
    struct SceneActorTargetMotionContext *c = a->base.context;
    if (!TakePrepared(a)) {
        u32 four = 4;
        u32 *address = &c->flags;
        u32 value = *address & four;
        flags = address;
        if (IsActive(value)) {
            if (a->base.tracking.distance > c->timeLimit) {
                SetPhase(a, 19);
                {
                    s32 angle = a->base.tracking.heading;
                    if (angle >= 0)
                        angle >>= 5;
                    else
                        angle = -(-angle >> 5);
                    a->base.nextDirection = (angle + 3) & 7;
                }
            } else {
                s32 mask = -5;
                *flags &= mask;
                if (c->phase <= 3) {
                    if ((u8)sub_080DEF78(a, a->base.body.direction))
                        SetPhase(a, 17);
                } else {
                    SetPhase(a, 19);
                    {
                        s32 angle = a->base.tracking.heading;
                        if (angle >= 0)
                            angle >>= 5;
                        else
                            angle = -(-angle >> 5);
                        a->base.nextDirection = (angle + 3) & 7;
                    }
                }
            }
        } else
            SceneActorAnimation_SelectDirectionalEntry(&a->base, 1);
    }
}
