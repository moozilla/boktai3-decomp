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
static inline void SetBehavior(struct SceneActorRegistryResetView *p, u32 b)
{
    p->state2A8 = 0;
    p->phase2A9 = 0;
    p->behavior2AA = b;
    p->counter2AC = 0;
    p->field2B1 = 1;
}
struct VornWaitTimer {
    u16 value : 16;
};
// confidence=low: waits for pursuit distance or timeout before restoring behavior 15.
#define Vorn_UpdatePursuitWait sub_080ADB90
void Vorn_UpdatePursuitWait(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *a = input;
    u8 *context = a->context;
    u16 *flags;
    if (SceneActorControl_TakeFlag(&a->field2B2)) {
        *(u16 *)(context + 0x69C) = 0;
    } else if (Pending(context, &flags)) {
        if (*(u32 *)((u8 *)a + 0xF0) <= 0x18FFF) {
            sub_080B1FA8(a);
            {
                s32 mask = ~2;
                ((struct ChaseFlags40 *)flags)->bits &= mask;
            }
            Chase_SetState(a, 0, 0, 15);
        } else {
            struct VornWaitTimer *timer;
            a->body.direction = *((u8 *)a + 0xE9);
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
