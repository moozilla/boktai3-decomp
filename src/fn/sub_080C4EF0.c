#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_restoration.h"

extern const u8 gUnk_0824E274[];

void Golem_ResetIdleControl(struct ChaseActor *actor)
{
    SceneActorReaction_ResetResponseState(&actor->group.saved.control, gUnk_0824E274, 0x22F);
    {
        u32 phase = 2;
        actor->base.state2A8 = 0;
        actor->base.phase2A9 = phase;
        actor->base.behavior2AA = 0;
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
    }
    Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
}
