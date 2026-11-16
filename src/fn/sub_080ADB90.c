#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"
void sub_080B1FA8(void *);
static inline u8 Pending(u8 *context, u16 **out)
{
    u32 mask = 2;
    u16 *flags = (void *)(context + 0x6D4);
    u32 value = *flags & mask;
    *out = flags;
    if (value)
        return 1;
    return 0;
}
static inline void SetBehavior(struct ChaseActor *p, u32 b)
{
    p->base.state2A8 = 0;
    p->base.phase2A9 = 0;
    p->base.behavior2AA = b;
    p->base.counter2AC = 0;
    p->base.field2B1 = 1;
}
struct VornWaitTimer {
    u16 value : 16;
};
// confidence=low: waits for pursuit distance or timeout before restoring behavior 15.
#define Vorn_UpdatePursuitWait sub_080ADB90
void Vorn_UpdatePursuitWait(struct ChaseActor *input)
{
    struct ChaseActor *a = input;
    u8 *context = a->base.context;
    u16 *flags;
    if (SceneActorControl_TakeFlag(&a->base.field2B2)) {
        *(u16 *)(context + 0x69C) = 0;
    } else if (Pending(context, &flags)) {
        if (a->base.tracking.distance <= 0x18FFF) {
            sub_080B1FA8(a);
            {
                s32 mask = ~2;
                ((struct ChaseFlags40 *)flags)->bits &= mask;
            }
            Chase_SetState(a, 0, 0, 15);
        } else {
            struct VornWaitTimer *timer;
            a->base.body.direction = *((u8 *)a + 0xE9);
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            timer = (void *)(context + 0x69C);
            ++timer->value;
            if (timer->value > 120) {
                sub_080B1FA8(a);
                {
                    s32 mask = ~2;
                    ((struct ChaseFlags40 *)flags)->bits &= mask;
                }
                Chase_SetState(a, 0, 0, 15);
                timer->value = 0;
            }
        }
    } else
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 1);
}
