#include "scene/actor/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "scene/actor/scene_actor_chase_shared.h"
#include "scene/actor/scene_actor_reset.h"

void TentacleBody_UpdateSpreadWindup(void);
void TentacleActor_SelectWanderRoute(struct ChaseActor *);
static inline u8 TakeQueuedFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void SceneActorPlacement_UpdateFlaggedResponse(struct ChaseActor *p)
{
    if (TakeQueuedFlag(&p->base.field2B1)) {
        SceneActorChase_SetControl(p, TentacleBody_UpdateSpreadWindup, 38);
        MonsterInit_SetStatusFlags(p, 0x100000);
        Sound_PlaySong(0x19B);
    }
    if (p->base.controlKind == 39 && (u8)Callback_InvokeActor(p, p->base.contactQuery.opaque)) {
        u32 two = 2;
        u8 *field = &p->base.state2A8;
        u32 zero = 0;
        *field = zero;
        p->base.phase2A9 = two;
        p->base.behavior2AA = zero;
        p->base.counter2AC = zero;
        p->base.field2B1 = 1;
    } else if (p->base.stepDone) {
        u32 two;
        u32 zero;
        u8 *field;
        TentacleActor_SelectWanderRoute(p);
        two = 2;
        field = &p->base.state2A8;
        zero = 0;
        *field = zero;
        p->base.phase2A9 = zero;
        p->base.behavior2AA = two;
        p->base.counter2AC = zero;
        p->base.field2B1 = 1;
        {
            u32 *flags = &p->base.statusFlags;
            u32 mask = ~0x100000;
            *flags &= mask;
        }
    } else {
        p->base.counter2AC++;
    }
}
