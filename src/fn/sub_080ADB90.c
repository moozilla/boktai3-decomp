// MATCHING_CONCESSION: register-pinning
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
            s32 mask;
            register u8 zero asm("r2");
            u32 wordZero;
            u32 behavior;
            register u32 offset asm("r5");
            sub_080B1FA8(a);
            mask = ~2;
            mask &= *flags;
            zero = 0;
            wordZero = 0;
            *flags = mask;
            behavior = 15;
            offset = 0x2A8;
            *((u8 *)a + offset) = zero;
            offset++;
            *((u8 *)a + offset) = zero;
            a->behavior2AA = behavior;
            offset += 3;
            *(u32 *)((u8 *)a + offset) = wordZero;
            asm volatile("" : "+r"(offset));
            a->field2B1 = 1;
        } else {
            u16 *timer;
            u32 wordZero;
            u8 zero;
            u32 behavior;
            {
                u32 heading = *((u8 *)a + 0xE9);
                u8 *direction = &a->body.direction;
                register u32 initial asm("r2") = 0;
                wordZero = initial;
                *direction = heading;
            }
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            {
                u32 offset = 0x69C;
                timer = (void *)(context + offset);
            }
            {
                u32 value = *timer + 1;
                zero = 0;
                *timer = value;
            }
            if (*timer > 120) {
                s32 mask;
                sub_080B1FA8(a);
                mask = ~2;
                *flags = mask & *flags;
                behavior = 15;
                a->state2A8 = zero;
                a->phase2A9 = zero;
                a->behavior2AA = behavior;
                a->counter2AC = wordZero;
                a->field2B1 = 1;
                *timer = wordZero;
            }
        }
    } else
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 1);
}
