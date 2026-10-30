// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
// confidence=low: repeats delayed animations before dispatching a context script.
extern const u8 gUnk_0824E224[];
void sub_080B3110(void *, u32, const u8 *, u32);
void SceneActor_UpdateDelayedAnimation(void);
static inline u32 RetryDuration(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 value;
    gRandomIndex = index;
    index *= 2;
    value = *(const u16 *)(index + (u32)gRandomTable);
    value &= 31;
    return value + 10;
}
static inline u32 NextRetryDuration(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 value;
    gRandomIndex = index;
    index *= 2;
    {
        register const u16 *table asm("r2") = gRandomTable;
        value = *(const u16 *)(index + (u32)table);
    }
    value &= 31;
    return value + 10;
}
void Vorn_UpdateAnimationRetries(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    if (Chase_TakeArrived(actor)) {
        Chase_SetCtl(actor, SceneActor_UpdateDelayedAnimation, 0x24);
        context[0x696] = 0;
        context[0x697] = RetryDuration();
        *(u16 *)(context + 0x694) = 0;
    }
    Callback_InvokeActor(actor, actor->contactQuery.opaque);
    if (actor->field2B0) {
        u8 *retries = context + 0x696;
        if (*retries) {
            Chase_SetCtl(actor, SceneActor_UpdateDelayedAnimation, 0x24);
            --*retries;
            context[0x697] = NextRetryDuration();
            ++*(u16 *)(context + 0x694);
        } else {
            u16 *flags = (void *)((u8 *)actor->context + 0x6D4);
            s32 mask = ~3;
            *flags &= mask;
            sub_080B3110(actor, 1, gUnk_0824E224, 0x41);
        }
    } else
        actor->counter2AC++;
}
