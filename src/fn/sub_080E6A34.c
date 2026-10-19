#include "global.h"
#include "scene_actor_reset.h"
#ifdef NONMATCHING
// fuzzy: 88.6%
// foundry draft (worker best 87.6%); what differs is mostly register allocation and instruction order
#define SceneActor_StartFlaggedAnimationPhase sub_080E6A34
s32 Callback_InvokeActor(void *, void *);
void SceneActor_WaitForCategoryAttachment(void);
static inline u8 TakeEntry(u8 *flag)
{
    if (*flag) { *flag = 0; return 1; }
    return 0;
}
void sub_080E6A34(struct SceneActorRegistryResetView *actor)
{
    if (TakeEntry(&actor->field2B1)) {
        u32 *motion = &actor->fieldDC;
        u32 zero = 0;
        struct SceneActorRegistryResetView *context;
        u32 one;
        u16 *animationFlags;
        u8 zero8;
        u32 value;
        void *target;
        u8 kind;
        u32 *flags;
        s32 mask;
        *motion = zero;
        context = actor->context;
        one = 1;
        animationFlags = &context->animationFlags65C;
        value = *animationFlags;
        zero8 = 0;
        *animationFlags = one | value;
        target = SceneActor_WaitForCategoryAttachment;
        kind = 0x25;
        actor->field2B2 = one;
        actor->field2B0 = zero8;
        actor->field2B5 = kind;
        actor->target = target;
        actor->field2C8 = zero;
        flags = &actor->flags114;
        mask = ~1;
        *flags &= mask;
        actor->word104 = zero;
    }
    if (actor->field2B0) {
        u32 behavior = 2;
        u32 zero;
        u8 *state = &actor->state2A8;
        zero = 0;
        *state = zero;
        actor->phase2A9 = zero;
        actor->behavior2AA = behavior;
        actor->counter2AC = zero;
        actor->field2B1 = 1;
    } else {
        Callback_InvokeActor(actor, actor->update270);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080E6A34);
#endif
