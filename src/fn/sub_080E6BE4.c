// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
// confidence=low: resolves the shadowless approach against the saved context position.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
void SceneActor_EnterShadowlessPhase(void);
void SceneActorControl_OnIdle(void);
u8 SceneActor_IsCollisionTargetUnavailable(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
static inline u8 TestContext(u8 *context, u32 mask)
{
    return Chase_Test(*(u16 *)(context + 0x65C), mask);
}
inline static void Chase_SetBehavior_1(struct SceneActorRegistryResetView *actor)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    {
        u8 k1 = 1;
        actor->field2B1 = k1;
    }
}

void Dog_UpdateShadowlessApproach(struct SceneActorRegistryResetView *actor)
{
    register u8 *group asm("r8") = (u8 *)(&actor->group);
    u8 *context = actor->context;
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, SceneActor_EnterShadowlessPhase, 0x24);
    if (SceneActorBehavior_RequestDescriptorTurn(actor, *((void **)(group + 0x1FC)), 32, 32, 0)) {
        *((u16 *)(((u8 *)actor) + 0x2EC)) = 0;
        *((u16 *)(((u8 *)actor) + 0x2F0)) = 0;
    }
    if (actor->field2B0) {
        u32 active = TestContext(context, 1);
        if (active) {
            if ((*((u16 *)(context + 0x658))) == (*((u16 *)(((u8 *)actor) + 0x2F4)))) {
                ((struct ChaseFlags40 *)(context + 0x65C))->bits &= ~1;
                Chase_SetBehavior_1(actor);
            } else
                Chase_SetBehavior(actor, 15);
        } else if (SceneActor_IsCollisionTargetUnavailable(actor)) {
            Chase_SetState(actor, 3, 0, 0);
            Chase_SetCtl(actor, SceneActorControl_OnIdle, 16);
            *((u16 *)(group + 0x44)) = 0;
        } else
            Chase_SetBehavior(actor, 15);
    } else
        Callback_InvokeActor(actor, actor->update270);
}

#else
INCLUDE_ASM("asm/nonmatching", Dog_UpdateShadowlessApproach);
#endif
