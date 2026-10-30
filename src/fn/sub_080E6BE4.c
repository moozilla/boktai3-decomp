// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
// confidence=low: resolves the shadowless approach against the saved context position.
void SceneActor_EnterShadowlessPhase(void);
void SceneActorControl_OnIdle(void);
u8 SceneActor_IsCollisionTargetUnavailable(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
struct DogApproachContext {
    u8 unknown0[0x658];
    u16 terrainCell;
    u8 unknown65A[2];
    u16 flags;
};
static inline u8 Dog_TestApproachFlag(struct DogApproachContext *context, u32 mask, u16 **flags)
{
    u16 *field = &context->flags;
    u32 value = *field & mask;
    *flags = field;
    if (value)
        return 1;
    return 0;
}

void Dog_UpdateShadowlessApproach(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    struct DogApproachContext *context = actor->context;
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, SceneActor_EnterShadowlessPhase, 0x24);
    if (SceneActorBehavior_RequestDescriptorTurn(actor, *(void **)(group + 0x1FC), 32, 32, 0)) {
        actor->secondaryMotion.x = 0;
        actor->secondaryMotion.z = 0;
    }
    if (actor->field2B0) {
        u16 *flags;
        u32 active = Dog_TestApproachFlag(context, 1, &flags);
        if (active) {
            if (context->terrainCell == actor->terrainCell) {
                u32 bit = 1;
                u32 mask = ~bit;
                /* Keep the bit and its complement available for the flag update. */
                asm("" : "+r"(bit), "+r"(mask) : : "r0", "r3");
                ((struct ChaseFlags40 *)flags)->bits &= mask;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = 0;
                actor->counter2AC = 0;
                actor->field2B1 = bit;
            } else
                Chase_SetBehavior(actor, 15);
        } else if (SceneActor_IsCollisionTargetUnavailable(actor)) {
            Chase_SetState(actor, 3, 0, 0);
            Chase_SetCtl(actor, SceneActorControl_OnIdle, 16);
            *(u16 *)(group + 0x44) = 0;
        } else
            Chase_SetBehavior(actor, 15);
    } else
        Callback_InvokeActor(actor, actor->contactQuery.opaque);
}
