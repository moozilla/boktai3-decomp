#include "scene_actor_chase.h"
#define FIELD(p, n, T) (*(T *)((u8 *)(p) + (n)))
extern const u8 gUnk_0824E2A8[];
u8 sub_080DEF78(struct SceneActorRegistryResetView *, u32);
u8 Dog_IsActionTimerExpired(struct SceneActorRegistryResetView *);
u8 SceneActorTarget_TrySelectNearestParticipant(struct SceneActorRegistryResetView *);
void SceneActorMotion_TestTargetTransition(void);
void Dog_SelectRetreatOrChase(struct SceneActorRegistryResetView *a)
{
    u8 *context = a->context;
    u8 *group = (u8 *)&a->group.common;
    SceneActorReaction_ResetResponseState(group + 0x8C, gUnk_0824E2A8, 0x2AA);
    if (FIELD(context, 0x6A7, u8) <= 3) {
        if (a->tracking.distance < FIELD(context, 0x688, u32) || !Dog_IsActionTimerExpired(a)) {
            if (FIELD(context, 0x6A7, u8) <= 3) {
                u32 saved = a->body.direction;
                a->body.direction = a->tracking.heading + 128;
                if (sub_080DEF78(a, saved)) {
                    Chase_SetCtl(a, SceneActorControl_OnIdle, 0x10);
                    Chase_SetBehavior(a, 17);
                }
                return;
            }
        }
        if (a->tracking.distance > FIELD(context, 0x68C, u32)) {
            struct ChaseTarget *target;
            if (a->field2B5 == 0x1A)
                return;
            if ((*(u32 *)&a->state2A8 & 0xFFFFFF) == 0xC0000)
                return;
            if (Chase_Test(gEngineFlags, 0x4800)) {
                if (!SceneActorTarget_TrySelectNearestParticipant(a))
                    return;
            } else
                a->tracking.target = (u32)((u8 *)gPlayers[0] + 0x24);
            {
                void *route = *(void **)(group + 0x1FC);
                struct ContactPosition *from = &a->body.position;
                struct ContactPosition *to;
                target = (void *)a->tracking.target;
                to = &target->position;
                if (!Chase_TryRoute(a, route, from, to)) {
                    Callback_InvokeActor(a, *(void **)(group + 0x224));
                    return;
                }
            }
            Chase_SetCtl(a, *(void **)(group + 0x48), 0x1A);
            Chase_SetBehavior(a, 12);
            return;
        }
    }
    {
        u32 state = 3;
        u32 phase = 1;
        u8 *first = &a->state2A8;
        u32 zero = 0;
        *first = state;
        a->phase2A9 = phase;
        a->behavior2AA = zero;
        a->counter2AC = zero;
        a->field2B1 = phase;
    }
    Chase_SetCtl(a, SceneActorMotion_TestTargetTransition, 0x2B);
}
