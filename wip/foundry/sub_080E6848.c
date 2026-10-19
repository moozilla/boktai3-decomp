#include "global.h"
#include "scene_actor_reset.h"
#define SceneActor_ResetMovementPhase sub_080E6848
s32 sub_0824923C(void *, void *);
static inline u8 TakeEntry(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void sub_080E6848(struct SceneActorRegistryResetView *actor)
{
    if (TakeEntry(&actor->field2B1)) {
        u32 zero8;
        u16 zero;
        u16 *flags;
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        u32 mask;
        {
            u16 *field = &actor->field2C2;
            zero8 = 0;
            zero = zero8;
            *field = 64;
        }
        actor->group.flags414 = zero;
        (*(u16 *)((u8 *)actor + 0x418)) = zero;
        flags = &actor->flags2D4;
        {
            u32 m = ~0x380;
            u32 v = *flags;
            m &= v;
            *flags = m;
        }
        motion = (void *)&actor->fieldDC;
        {
            u32 m = ~0x18;
            u32 v = motion->flags;
            m &= v;
            motion->flags = m;
        }
        control = (void *)((u8 *)actor + 0x420);
        mask = 0x200000;
        if (mask & *(u32 *)((u8 *)actor + 0x2CC)) {
            u32 *word = (void *)((u8 *)actor + 0x2D0);
            mask = ~0x1000000;
            *word &= mask;
            {
                u32 m = ~0x200;
                u32 v = *flags;
                m &= v;
                *flags = m;
            }
            control->mode = zero8;
            control->counter = zero;
            control->timer = zero;
        }
        {
            u32 m = ~0x400;
            u32 v = *flags;
            m &= v;
            *flags = m;
        }
        actor->field5DA = 0xFF;
        motion->fieldDC = zero;
        {
            void *target = actor->savedTarget;
            u32 *word;
            actor->field2B2 = 1;
            actor->field2B0 = zero8;
            actor->field2B5 = zero8;
            *(void **)((u8 *)actor + 0x21C) = target;
            {
                u16 *timer = (void *)((u16 *)actor + 0x164);
                *timer = zero;
            }
            word = &actor->flags114;
            mask = ~1;
            *word &= mask;
            actor->word104 = zero;
        }
    }
    sub_0824923C(actor, actor->update270);
}
