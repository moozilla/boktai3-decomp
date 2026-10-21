// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
s32 Callback_InvokeActor(void *, void *);
void SceneActor_WaitForCategoryAttachment(void);
static inline u8 TakeEntry(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void SceneActor_StartFlaggedAnimationPhase(struct SceneActorRegistryResetView *actor)
{
    if (TakeEntry(&actor->field2B1)) {
        u32 *motion = &actor->fieldDC;
        register u32 zero asm("r4") = 0;
        struct SceneActorRegistryResetView *context;
        register u32 one asm("r3");
        register u16 *animationFlags asm("r1");
        register u8 zero8 asm("r5");
        register u32 value asm("r2");
        void *target;
        u8 kind;
        u32 *flags;
        s32 mask;
        asm("" : "+r"(zero));
        *motion = zero;
        context = actor->context;
        one = 1;
        asm("" : "+r"(one));
        animationFlags = &context->animationFlags65C;
        value = *animationFlags;
        zero8 = 0;
        asm("" : "+r"(zero8));
        {
            u32 t;
            asm("" : "=r"(t) : "0"(one));
            *animationFlags = t | value;
        }
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
