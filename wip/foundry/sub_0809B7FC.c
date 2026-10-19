#define ActorBehavior_FinishTimedTransition sub_0809B7FC
#define ActorMovement_BeginTowardPosition   sub_0809D0C8
#define gPlayerSlots                        gUnk_02000580
#include "global.h"
#include "scene_actor_reset.h"
struct ActorTransitionState {
    u8 unknown0[0x442];
    u16 resetTimer, timer;
};
extern u8 *gUnk_02000580;
void sub_0809D4EC(struct SceneActorRegistryResetView *);
void sub_0809D044(struct SceneActorRegistryResetView *);
void sub_0809D0C8(u8 *, s16 *);
void sub_0809B6AC(void);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *a)
{
    if (a->field2B2) {
        a->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline void FinishTransition(struct SceneActorRegistryResetView *a, s32 elapsed, u8 zero)
{
    if (elapsed > 30) {
        struct ActorTransitionState *s = a->context;
        u32 timer;
        sub_0809D044(a);
        timer = s->timer;
        if (timer) {
            {
                u32 mask = 0x100;
                u16 *flags = &a->flags2D4;
                mask |= *flags;
                *flags = mask;
            }
            s->timer = s->resetTimer;
            sub_0809D0C8((void *)a, (void *)(gUnk_02000580 + 0x30));
            {
                u32 two = 2;
                u32 one = 1;
                a->state2A8 = zero;
                a->phase2A9 = two;
                a->behavior2AA = one;
                a->counter2AC = zero;
                a->field2B1 = one;
            }
        } else {
            void *callback = sub_0809B6AC;
            u32 kind = 10;
            u32 one;
            {
                u32 offset = 0x2B2;
                u8 *p = (u8 *)a + offset;
                one = 1;
                *p = one;
            }
            {
                u32 offset = 0x2B0;
                u8 *p = (u8 *)a + offset;
                *p = zero;
                offset += 5;
                p = (u8 *)a + offset;
                *p = kind;
            }
            a->target = callback;
            a->field2C8 = timer;
            {
                u32 *flags = &a->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            a->word104 = timer;
            a->state2A8 = zero;
            a->phase2A9 = one;
            a->behavior2AA = one;
            a->counter2AC = timer;
            a->field2B1 = one;
            a->direction = a->fieldE9;
            a->field2C2 = 0x40;
        }
    }
}
void sub_0809B7FC(struct SceneActorRegistryResetView *a, s32 elapsed)
{
    if (TakePrepared(a))
        sub_0809D4EC(a);
    {
        u32 mask = 0x10;
        u16 *flags = &a->flags2D6;
        u32 old = *flags;
        u8 zero = 0;
        mask |= old;
        *flags = mask;
        {
            u32 mask = 0x100000;
            u32 *flags = &a->flags2D0;
            *flags |= mask;
        }
        FinishTransition(a, elapsed, zero);
    }
}
